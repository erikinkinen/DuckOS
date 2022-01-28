#include <utils/bitmap.h>

unsigned long long Bitmap::size() {
    return m_size;
}

void Bitmap::setSize(unsigned long long size) {
    m_size = size;
}

void *Bitmap::buffer() {
    return (void *)m_buffer;
}

void Bitmap::setBuffer(void *buffer) {
    m_buffer = (unsigned char *)buffer;
}

bool Bitmap::operator[](unsigned long long index){
    if ((m_buffer[index / 8] & (1 << (index % 8))) > 0) return true;
    else return false;
}

void Bitmap::set(unsigned long long index, bool value){
    if (value == true) m_buffer[index / 8] |= 1 << (index % 8);
    else m_buffer[index / 8] &= ~(1 << (index % 8));
}
