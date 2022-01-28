#include <drivers/serial.h>
#include <drivers/x86serial.h>
#include <drivers/ports.h>

SerialPort::SerialPort(unsigned short port) : port(SERIAL_PORT_COM1) {}
SerialPort::~SerialPort() {}

int SerialPort::init() {
    // Disable interrupts
    outb(port + 1, 0x00);

    // Enable DLAB and set divisor to 3 (38400 baud)
    outb(port + 3, 0x80);
    outb(port + 0, 0x03);
    outb(port + 1, 0x00);

    // 8-bit data, no parity, one stop bit
    outb(port + 3, 0x03);

    // Enable FIFO
    outb(port + 2, 0xC7);

    // IRQs enabled
    outb(port + 4, 0x0B);

    // Loopback test
    outb(port + 4, 0x1E);
    outb(port + 0, 0xAE);

    if (inb(port + 0) != 0xAE) 
        return -1;

    // Normal operation mode
    outb(port + 4, 0x0F);

    return 0;
}

void SerialPort::write_char(char a) {
    // Is transmit empty?
    while (inb(port + 5) & 0x20 == 0);

    // Send byte
    outb(port, a);
}

void SerialPort::write_str(const char *str) {
    while (*str != 0) {
        write_char(*str);
        str++;
    }
}