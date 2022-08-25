#include <fs/devfs.h>
#include <utils/logger.h>

const char *ConsoleFile::default_name = "console";

ConsoleFile::ConsoleFile(FSNode *parent) 
    : AbstractFile(parent) {}

unsigned long long ConsoleFile::get_size() {
    return 0;
}

signed long long ConsoleFile::write(const void *buf, unsigned long long nbytes) {
    for (char *chr = (char *)buf; chr < (char *)buf + nbytes; chr++) {
        if (*chr == '\0') {
            continue;
        }

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
    return nbytes;
}

signed long long ConsoleFile::read(void *buf, unsigned long long nbytes) {
    return -1;
}

DevFS::DevFS(FSNode *parent) : AbstractFS(parent) {
    ConsoleFile *console = new ConsoleFile(this);
    console->name = (char*)ConsoleFile::default_name;
    children->append(console);
}