#include <efi.h>
#include <efilib.h>
#include <elf.h>

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

#define ERR_MSG L"Could not load DuckOS!\n"

typedef unsigned long long size_t;

typedef struct {
	void* BaseAddress;
	size_t BufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelsPerScanLine;
} Framebuffer;

typedef struct {
	EFI_MEMORY_DESCRIPTOR *Base;
	UINTN DescCount;
	UINTN DescSize;
} MemMapPtr;

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
	Framebuffer framebuffer;
	MemMapPtr memmap;
	PSF1Font *font;
	void *initrd;
	unsigned long long initrd_size;
} BootInfo;

BootInfo boot_info;

int memcmp(const void *a, const void* b, size_t n) {
	for (size_t i = 0; i < n; ++i) {
		if (((char*)a)[i] < ((char*)b)[i]) return -1;
		if (((char*)a)[i] > ((char*)b)[i]) return 1;
	}

	return 0;
}

EFI_STATUS init_gop() {
	EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
	EFI_STATUS status;

	status = uefi_call_wrapper(BS->LocateProtocol, 3, &gop_guid, NULL, (void **)&gop);
	if (EFI_ERROR(status)) return EFI_LOAD_ERROR;
	
	boot_info.framebuffer.BaseAddress = (void*)gop->Mode->FrameBufferBase;
	boot_info.framebuffer.BufferSize = gop->Mode->FrameBufferSize;
	boot_info.framebuffer.Width = gop->Mode->Info->HorizontalResolution;
	boot_info.framebuffer.Height = gop->Mode->Info->VerticalResolution;
	boot_info.framebuffer.PixelsPerScanLine = gop->Mode->Info->PixelsPerScanLine;

	return EFI_SUCCESS;
}

EFI_FILE *load_file(EFI_FILE *dir, CHAR16 *path, EFI_HANDLE img_handle, EFI_SYSTEM_TABLE *st) {
	EFI_FILE *loaded_file;

	EFI_LOADED_IMAGE_PROTOCOL *loaded_img;
	st->BootServices->HandleProtocol(img_handle, &gEfiLoadedImageProtocolGuid, (void **)&loaded_img);

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;
	st->BootServices->HandleProtocol(loaded_img->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void **)&fs);

	if (dir == NULL) {
		fs->OpenVolume(fs, &dir);
	}

	EFI_STATUS status = dir->Open(dir, &loaded_file, path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if (status != EFI_SUCCESS) return NULL;
	return loaded_file;
}

PSF1Font* LoadPSF1Font(EFI_FILE *dir, CHAR16 *path, EFI_HANDLE img_handle, EFI_SYSTEM_TABLE *st)
{
	EFI_FILE* font = load_file(dir, path, img_handle, st);
	if (font == NULL) return NULL;

	PSF1Header* font_header;
	UINTN size = sizeof(PSF1Header);
	st->BootServices->AllocatePool(EfiLoaderData, size, (void**)&font_header);
	font->Read(font, &size, font_header);

	if (font_header->magic[0] != PSF1_MAGIC0 || font_header->magic[1] != PSF1_MAGIC1){
		return NULL;
	}

	UINTN glyph_buffer_size = font_header->charsize * 256;
	if (font_header->mode == 1) {
		glyph_buffer_size = font_header->charsize * 512;
	}

	void* glyph_buffer;
	{
		font->SetPosition(font, sizeof(PSF1Header));
		st->BootServices->AllocatePool(EfiLoaderData, glyph_buffer_size, (void**)&glyph_buffer);
		font->Read(font, &glyph_buffer_size, glyph_buffer);
	}

	PSF1Font* finished_font;
	st->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1Font), (void**)&finished_font);
	finished_font->psf1_header = font_header;
	finished_font->glyph_buffer = glyph_buffer;
	return finished_font;

}

EFI_STATUS efi_main (EFI_HANDLE img_handle, EFI_SYSTEM_TABLE *st) {
	InitializeLib(img_handle, st);
	
	EFI_FILE *kernel = load_file(NULL, L"kernel.duck", img_handle, st);

	if (kernel == NULL) {
		Print(ERR_MSG);
		for (;;);
	}

	Elf64_Ehdr header;
	UINTN header_size = sizeof(header);
	kernel->Read(kernel, &header_size, &header);

	if (
		memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
		header.e_ident[EI_CLASS] != ELFCLASS64 ||
		header.e_type != ET_EXEC ||
		header.e_version != EV_CURRENT
	) {
		Print(ERR_MSG);
		for (;;);
	};

	Elf64_Phdr *phdrs;
	kernel->SetPosition(kernel, header.e_phoff);
	UINTN phdr_size = header.e_phnum * header.e_phentsize;
	st->BootServices->AllocatePool(EfiLoaderData, phdr_size, (void **)&phdrs);
	kernel->Read(kernel, &phdr_size, phdrs);

	for (
		Elf64_Phdr* phdr = phdrs; 
		(char *)phdr < (char *)phdrs + header.e_phnum * header.e_phentsize;
		phdr = (Elf64_Phdr *)((char *)phdr + header.e_phentsize)
	) {
		switch (phdr->p_type) {
			case PT_LOAD: {
				int pages = (phdr->p_memsz + 0xFFF) / 0x1000;
				Elf64_Addr segment = phdr->p_paddr;
				st->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);
				kernel->SetPosition(kernel, phdr->p_offset);
				UINTN size = phdr->p_filesz;
				kernel->Read(kernel, & size, (void *)segment);
				break;
			}
		}
	}

	EFI_FILE *initrd = load_file(NULL, L"INITRD.TAR", img_handle, st);

	if (initrd != NULL) {
		EFI_FILE_INFO *initrd_file_info = LibFileInfo(initrd);
		boot_info.initrd_size = initrd_file_info->FileSize;
		st->BootServices->AllocatePool(EfiLoaderData, initrd_file_info->FileSize, (void **)&(boot_info.initrd));
		initrd->Read(initrd, &initrd_file_info->FileSize, boot_info.initrd);
		FreePool(initrd_file_info);
	} else boot_info.initrd = NULL;

	boot_info.font = LoadPSF1Font(NULL, L"FONT.PSF", img_handle, st);
	if (boot_info.font == NULL){
		Print(ERR_MSG);
		for (;;);
	}

	if (init_gop() != EFI_SUCCESS){
		Print(ERR_MSG);
		for (;;);
	}

	UINTN mapKey = 0;
	UINT32 descVersion = 0;
	boot_info.memmap.Base = LibMemoryMap(&(boot_info.memmap.DescCount), &mapKey, &(boot_info.memmap.DescSize), &descVersion);

	int (* kernel_main)(BootInfo *) = ((__attribute__((sysv_abi)) int (*)(BootInfo *)) header.e_entry);
	st->BootServices->ExitBootServices(img_handle, mapKey);
	kernel_main(&boot_info);

	return EFI_SUCCESS;
}
