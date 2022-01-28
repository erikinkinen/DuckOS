#include <utils/logger.h>

SerialPort *com1_ptr;
char buffer[256];
unsigned int cursor_x, cursor_y;
unsigned int console_scale_x = 1;
unsigned int console_scale_y = 1;

void Logger::init(SerialPort *com1) {
    com1_ptr = com1;
    if (com1_ptr) com1_ptr->init();

    put_rect(0, 0, 0, bInfo.fb.width, bInfo.fb.height);
    if (com1_ptr) com1_ptr->write_str("\x1B[2J\x1B[H");

    Logger::info("DuckKernel Debug Log\r\n============================\r\n");
}

void Logger::info(const char *str) {
    if (com1_ptr) com1_ptr->write_str(str);
    print(str, 0xFFFFFF);
}

void Logger::warn(const char *str) {
    if (com1_ptr) com1_ptr->write_str(str);
    print(str, 0xFFFF00);
}

void Logger::error(const char *str) {
    if (com1_ptr) com1_ptr->write_str(str);
    print(str, 0xFF0000);
}

void Logger::ok() {
    if (com1_ptr) com1_ptr->write_str(" [OK!]\r\n");
    cursor_x = bInfo.fb.width - 56 * console_scale_x;
    print("[ OK! ]", 0x00FF00);
}

void Logger::report() {
    Logger::info("\r\nUsed RAM: ");
    Logger::warn(itoa(PageFrameAllocator::get_used() / 1024, buffer, 10));
    Logger::info(" KB\r\nFree RAM: ");
    Logger::warn(itoa(PageFrameAllocator::get_free() / 1024, buffer, 10));
    Logger::info(" KB\r\n\r\n");
}