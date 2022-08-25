#include <utils/logger.h>

SerialPort *Logger::serial = nullptr;

char buffer[256];
unsigned int cursor_x = 60, cursor_y = 40;
unsigned int console_scale_x = 1;
unsigned int console_scale_y = 1;

void Logger::init(SerialPort *serial) {
    Logger::serial = serial;
    if (Logger::serial) Logger::serial->init();

    put_rect(0, 0, 0, bInfo.fb.width, bInfo.fb.height);
    if (Logger::serial) Logger::serial->write_str("\x1B[2J\x1B[H");

    Logger::info("DuckKernel Debug Log\r\n============================\r\n");
}

void Logger::info(const char *str) {
    if (Logger::serial) Logger::serial->write_str(str);
    Logger::print_screen(str, 0xFFFFFF);
}

void Logger::warn(const char *str) {
    if (Logger::serial) Logger::serial->write_str(str);
    Logger::print_screen(str, 0xFFFF00);
}

void Logger::error(const char *str) {
    if (Logger::serial) Logger::serial->write_str(str);
    Logger::print_screen(str, 0xFF0000);
}

void Logger::ok() {
    if (Logger::serial) Logger::serial->write_str(" [OK!]\r\n");
    cursor_x = bInfo.fb.width - 116 * console_scale_x;
    Logger::print_screen("[ OK! ]", 0x00FF00);
}

void Logger::memory_report() {
    Logger::info("\nUsed RAM: ");
    Logger::info(itoa(PageFrameAllocator::get_used() / 1024, buffer, 10));
    Logger::info(" KB\nFree RAM: ");
    Logger::info(itoa(PageFrameAllocator::get_free() / 1024, buffer, 10));
    Logger::info(" KB\n\n");
}

void Logger::print_screen(const char *str, unsigned int color) {
    for (char *chr = (char *)str; *chr != 0; chr++) {
        if (*chr == '\r') {
            cursor_x = 60;
            continue;
        }
        
        if (*chr == '\n') {
            cursor_y += 16 * console_scale_y;
            cursor_x = 60;
            continue;
        }

        put_char(*chr, color, cursor_x, cursor_y);
        cursor_x += 8 * console_scale_x;
        if (cursor_x + 8 * console_scale_x > bInfo.fb.width - 60) {
            cursor_x = 60;
            cursor_y += 16 * console_scale_y;
        }
    }
}