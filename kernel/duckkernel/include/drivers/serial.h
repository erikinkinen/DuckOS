#pragma once

class SerialPort {
public:
    SerialPort(unsigned short port);
    ~SerialPort();

    int init();
    void write_char(char a);
    void write_str(const char *str);

private:
    unsigned short port;
};
