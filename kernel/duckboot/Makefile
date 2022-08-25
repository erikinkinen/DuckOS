DEBUG ?= false

CC = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-gcc
AS = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-as
AR = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-ar
CXX = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-g++
LD = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-ld
ifeq ($(ARCH), x86_64)
OBJCOPY = objcopy
BOOTNAME = bootx64.efi
else
$(error Unknown Architecture)
endif

.PHONY: all clean

all: $(BUILDDIR)/src
	@$(MAKE) -s -C $(BUILDDIR) -f boot.mk all CC=$(CC) AS=$(AS) LD=$(LD) OBJCOPY=$(OBJCOPY) CFLAGS="$(CFLAGS) -fshort-wchar -fPIC -I/usr/include"
	@mkdir -p $(SYSROOTDIR)/efi/boot/
	@cp -u $(CURDIR)/prebuilt/zap-light16.psf $(SYSROOTDIR)/font.psf
	@cp -u $(BUILDDIR)/main.efi $(SYSROOTDIR)/efi/boot/$(BOOTNAME)

$(BUILDDIR)/src:
	@mkdir -p $(BUILDDIR)
	@cp -r $(CURDIR)/* $(BUILDDIR)