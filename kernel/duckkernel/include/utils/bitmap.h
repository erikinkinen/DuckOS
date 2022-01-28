#pragma once

class Bitmap {
public:
    unsigned long long size();
    void setSize(unsigned long long size);

    void *buffer();
    void setBuffer(void *buffer);

    bool operator[](unsigned long long index);
    void set(unsigned long long index, bool value);
private:
    unsigned long long m_size;
    unsigned char *m_buffer;
};
