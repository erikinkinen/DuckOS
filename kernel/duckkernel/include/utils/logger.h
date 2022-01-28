#pragma once

#include <init/boot.h>
#include <drivers/serial.h>
#include <memory/pageframe.h>
#include <utils/utils.h>

extern char buffer[256];
extern unsigned int cursor_x, console_scale_x;
extern unsigned int cursor_y, console_scale_y;

inline void put_pixel(unsigned int color, unsigned int x, unsigned int y) {
    *(((unsigned int *)bInfo.fb.base_addr) + x + y * bInfo.fb.pps) = 0xFF000000 | color;
}

inline void put_rect(unsigned int color, unsigned int x, unsigned int y, unsigned int w, unsigned int h) {
    for (int xoff = 0; xoff < w; xoff++) {
        for (int yoff = 0; yoff < h; yoff++) {
            put_pixel(color, x + xoff, y + yoff);
        }
    }
}

inline void put_char(char chr, unsigned int color, unsigned int xOff, unsigned int yOff)
{
    char* fontPtr = (char*)bInfo.font->glyph_buffer + (chr * bInfo.font->psf1_header->charsize);
    for (unsigned long y = yOff; y < yOff + 16 * console_scale_y; y += console_scale_y){
        for (unsigned long x = xOff; x < xOff + 8 * console_scale_x; x += console_scale_x){
            put_rect(((*fontPtr & (0x80 >> (x - xOff)/console_scale_x)) > 0) ? color : 0, x, y, console_scale_x, console_scale_y);
        }
        fontPtr++;
    }
}

inline void print(const char *str, unsigned int color) {
    char *chr = (char *)str;
    while (*chr != 0) {
        if (*chr == '\r') {
            cursor_x = 0;
            chr ++;
            continue;
        }
        
        if (*chr == '\n') {
            cursor_y += 16 * console_scale_y;
            chr ++;
            continue;
        }

        put_char(*chr, color, cursor_x, cursor_y);
        cursor_x += 8 * console_scale_x;
        if (cursor_x + 8 * console_scale_x > bInfo.fb.width) {
            cursor_x = 0;
            cursor_y += 16 * console_scale_y;
        }
        chr ++;
    }
}

class Logger {
public:
    static void init(SerialPort *com1);
    static void info(const char *str);
    static void warn(const char *str);
    static void error(const char *str);
    static void ok();
    static void report();
};
