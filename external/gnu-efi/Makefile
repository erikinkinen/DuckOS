DEBUG ?= false

CC = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-gcc
AS = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-as
AR = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-ar
CXX = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-g++
LD = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-ld
ifeq ($(ARCH), x86_64)
OBJCOPY = objcopy
else
OBJCOPY = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-objcopy
endif

.PHONY: all clean

all: $(BUILDDIR)
	@$(MAKE) -C $(BUILDDIR) all CC=$(CC) AS=$(AS) LD=$(LD) OBJCOPY=$(OBJCOPY) CFLAGS="-fPIC -fshort-wchar -I/usr/include" >/dev/null

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)
	@[[ -f gnu-efi-3.0.14.tar.bz2 ]] || wget https://download.sourceforge.net/gnu-efi/gnu-efi-3.0.14.tar.bz2 -O gnu-efi-3.0.14.tar.bz2
	@tar -xf gnu-efi-3.0.14.tar.bz2 -C "$(BUILDDIR)/" --strip-components=1