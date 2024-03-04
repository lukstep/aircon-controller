import re

with open("/Users/lukstep/Projects/aircon-controler/logs/rs485_4800b_8bit_1stop_test_1.txt", 'r') as file:
    for line in file.readlines():
        print(line, end='')
        frame = re.search(r"#(\d{1,})", line)
        if frame:
            frame_number = frame.group(0).replace('#', '')
            print(f"frame:{frame_number}", end='  [')
            data = re.findall(r".*\[(?P<data>.*) \]", line)
            for i in data[0].split(' '):
                number = int(i, 16)
                print(f"{number}", end='')
                print(((len(i)+1) - len(str(number))) * " ", end='')
            print("]")
