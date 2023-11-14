#include <cstdint>
#include <ctime>

#include "lwip/dns.h"
#include "lwip/err.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "ntp.hpp"
#include "pico/cyw43_arch.h"

#include "logger.hpp"
#include "rtc.hpp"

namespace ntp
{

Client::Client()
{
    udp = udp_new_ip_type(IPADDR_ANY);
    if (!udp)
    {
        *logger << "Failed to create UDP\n";
        state = ClientState::NOT_INITIALIZED;
    }
    udp_recv(udp, packetRecvCallback, this);
    if (!sendDnsRequest())
    {
        *logger << "DNS request failed!\n";
    }
    else
    {
        state = ClientState::DNS_REQUEST_SENT;
    }
}

Client::~Client()
{
    udp_remove(udp);
    udp = nullptr;
}

bool Client::syncTime()
{
    while (true)
    {
        switch (state)
        {
        case ClientState::NOT_INITIALIZED:
            *logger << "[ERR] Client not properly initialized\n";
            return false;
        case ClientState::INITIALIZED:
        {
            if (!sendDnsRequest())
            {
                *logger << "[ERR] DNS request failed!\n";
            }
            else
            {
                state = ClientState::DNS_REQUEST_SENT;
            }
        }
        break;
        case ClientState::NTP_SERVER_IP_SET:
            if (sendRequest())
            {
                state = ClientState::NTP_REQUEST_SENT;
            }
            else
            {
                state = ClientState::NTP_SERVER_IP_SET;
                return false;
            }
            break;
        case ClientState::NTP_RESPONSE_RECEIVED:
            state = ClientState::NTP_SERVER_IP_SET;
            return true;
        default:
            sleep_ms(200);
            break;
        }
    }
}

void Client::packetRecvCallback(void *arg, [[maybe_unused]] struct udp_pcb *udp,
                                struct pbuf *buffer, const ip_addr_t *addres, uint16_t port)
{
    Client *ntp_ptr = static_cast<Client *>(arg);

    constexpr uint8_t mode_mask = 0x07;
    uint8_t mode = pbuf_get_at(buffer, 0) & mode_mask;
    uint8_t stratum = pbuf_get_at(buffer, 1);

    if (ip_addr_cmp(addres, &ntp_ptr->serverIp) && port == ntp_ptr->PORT &&
        buffer->tot_len == ntp::Client::NTP_MSG_LEN && mode == 0x4 && stratum != 0)
    {
        ntp_ptr->parseResponse(buffer);
    }
    else
    {
        *logger << "[ERR] Invalid NTP response\n";
    }
    pbuf_free(buffer);
}

void Client::serverDnsFoundCallback([[maybe_unused]] const char *hostname, const ip_addr_t *addres,
                                    void *arg)
{
    Client *client = static_cast<Client *>(arg);
    if (addres)
    {
        client->setAddres(*addres);
        *logger << rtc::getTime() << " [INF] NTP server IP addres " << ipaddr_ntoa(addres) << "\n";
    }
    else
    {
        *logger << "[ERR] DNS request failed\n";
    }
}

bool Client::sendRequest()
{
    bool status = true;
    cyw43_arch_lwip_begin();
    struct pbuf *buffer = pbuf_alloc(PBUF_TRANSPORT, NTP_MSG_LEN, PBUF_RAM);
    if (buffer != nullptr)
    {
        uint8_t *req = (uint8_t *)buffer->payload;
        memset(req, 0, NTP_MSG_LEN);
        req[0] = 0x1b;
        udp_sendto(udp, buffer, &serverIp, PORT);
        pbuf_free(buffer);
    }
    else
    {
        *logger << "[ERR] pbuf allocation failed!\n";
        status = false;
    }
    cyw43_arch_lwip_end();
    return status;
}

bool Client::sendDnsRequest()
{
    cyw43_arch_lwip_begin();
    int32_t err = dns_gethostbyname(SERVER_URL, &serverIp, serverDnsFoundCallback, this);
    cyw43_arch_lwip_end();
    return err == ERR_OK or err == ERR_INPROGRESS ? true : false;
}

void Client::parseResponse(struct pbuf *buffer)
{
    uint8_t seconds_buf[4];
    pbuf_copy_partial(buffer, seconds_buf, sizeof(seconds_buf), 40);
    uint32_t seconds_since_1900 =
        seconds_buf[0] << 24 | seconds_buf[1] << 16 | seconds_buf[2] << 8 | seconds_buf[3];
    std::time_t epoch = static_cast<timer_t>(seconds_since_1900 - NTP_DELTA);
    struct tm *utc_time = gmtime(&epoch);
    time.year = utc_time->tm_year + 1900;
    time.month = utc_time->tm_mon + 1;
    time.day = utc_time->tm_mday;
    time.hour = utc_time->tm_hour + 1;
    time.min = utc_time->tm_min;
    time.sec = utc_time->tm_sec;
    state = ClientState::NTP_RESPONSE_RECEIVED;
}
} // namespace ntp