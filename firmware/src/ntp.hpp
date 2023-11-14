#pragma once
#include "lwip/ip_addr.h"
#include "lwip/udp.h"
#include "pico/types.h"

namespace ntp
{

class Client
{
    enum class ClientState
    {
        NOT_INITIALIZED,
        INITIALIZED,
        DNS_REQUEST_SENT,
        NTP_SERVER_IP_SET,
        NTP_REQUEST_SENT,
        NTP_RESPONSE_RECEIVED,
    };

  protected:
    static constexpr int32_t NTP_MSG_LEN = 48;
    static constexpr uint32_t NTP_DELTA = 2'208'988'800;
    static constexpr uint16_t PORT = 123;
    static constexpr auto SERVER_URL = "pool.ntp.org";
    ClientState state = ClientState::INITIALIZED;
    struct udp_pcb *udp;
    ip_addr_t serverIp;
    datetime_t time = {.year = 0, .month = 0, .day = 0, .dotw = 0, .hour = 0, .min = 0, .sec = 0};

  public:
    Client();
    Client(const Client &) = delete;
    Client(const Client &&) = delete;
    Client &operator=(const Client &) = delete;
    Client &operator=(const Client &&) = delete;
    ~Client();
    bool syncTime();
    inline datetime_t *getTime() { return &time; }
    void parseResponse(struct pbuf *buffer);

    inline void setAddres(const ip_addr_t addres)
    {
        serverIp = addres;
        state = ClientState::NTP_SERVER_IP_SET;
    };

    static void packetRecvCallback(void *ntp_ptr, struct udp_pcb *udp, struct pbuf *buffer,
                                   const ip_addr_t *addres, uint16_t port);
    static void serverDnsFoundCallback(const char *hostname, const ip_addr_t *ipaddr, void *arg);

  private:
    inline bool sendRequest();
    inline bool sendDnsRequest();
};
} // namespace ntp