#pragma once

typedef struct {
	void *base_addr;
	unsigned long long size;
	unsigned int width;
	unsigned int height;
	unsigned int pps;
} FrameBuffer;

typedef struct {
	void *base_addr;
	long unsigned int descCount;
	long unsigned int descSize;
} MemmapPtr;

typedef struct {
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
} PSF1Header;

typedef struct {
	PSF1Header* psf1_header;
	void* glyph_buffer;
} PSF1Font;

typedef struct {
    FrameBuffer fb;
    MemmapPtr memmap;
	PSF1Font *font;
	void *initrd;
	unsigned long long initrd_size;
} BootInfo;

extern BootInfo bInfo;