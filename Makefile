OSNAME = DuckOS
ARCH ?= x86_64

BUILDDIR ?= $(CURDIR)/out/build/packages
SYSROOTDIR ?= $(CURDIR)/out/sysroot

TOOLBUILDDIR ?= $(CURDIR)/out/build/tools
TOOLROOTDIR ?= $(CURDIR)/out/toolchain

ifeq ($(ARCH), x86_64)
OVMFDIR ?= /usr/share/edk2-ovmf/x64
else
OVMFDIR ?= /usr/share/edk2-ovmf/$(ARCH)
endif

PROJECTS += \
	    external/gnu-efi \
	    kernel/duckboot \
		kernel/duckkernel \
		corelib/libc \
		init/duckinit \
		init/initrd

TOOLS += \
		corelib/libc \
		external/binutils \
		external/gcc

DEBUG ?= false

all: 
	@echo -e "\e[1;31mBuilding $(OSNAME)...\e[1;0m"
	@$(MAKE) -s $(addsuffix .all,$(PROJECTS)) DEBUG=$(DEBUG) ARCH=$(ARCH) BUILDDIR=$(BUILDDIR) TOOLROOTDIR=$(TOOLROOTDIR) SYSROOTDIR=$(SYSROOTDIR)

toolchain:
	@echo -e "\e[1;31mBuilding a toolchain for $(OSNAME)...\e[1;0m"
	@$(MAKE) -s $(addsuffix .hostall,$(TOOLS)) DEBUG=$(DEBUG) ARCH=$(ARCH) TOOLBUILDDIR=$(TOOLBUILDDIR) TOOLROOTDIR=$(TOOLROOTDIR) SYSROOTDIR=$(SYSROOTDIR)

clean:
	@echo -e "\e[1;31mCleaning $(OSNAME) build tree...\e[1;0m"
	@rm -rf $(BUILDDIR) $(SYSROOTDIR)

image: 
	@echo -e "\e[1;31mBuilding $(OSNAME) image...\e[1;0m"
	@dd if=/dev/zero of=$(BUILDDIR)/$(OSNAME).img bs=512 count=93750
	@mkfs.fat $(BUILDDIR)/$(OSNAME).img
	@mcopy -s -i $(BUILDDIR)/$(OSNAME).img $(SYSROOTDIR)/* ::/

run:
	@echo -e "\e[1;31mRunning $(OSNAME) in qemu emulator...\e[1;0m"
	@[[ -f "$(BUILDDIR)/OVMF_VARS.fd" ]] || cp $(OVMFDIR)/OVMF_VARS.fd $(BUILDDIR)/
	@qemu-system-$(ARCH) -drive format=raw,file=$(BUILDDIR)/$(OSNAME).img -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="$(OVMFDIR)/OVMF_CODE.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="$(BUILDDIR)/OVMF_VARS.fd" -net none -serial stdio

super: toolchain all image

superrun: super run

superclean:
	@echo -e "\e[1;31mCleaning up all build trees...\e[1;0m"
	@rm -rf $(CURDIR)/out

$(addsuffix .all,$(PROJECTS)):
	@echo -e "+-- \e[1;36mBuilding package: $(subst .all,,$@)\e[1;0m"
	@$(MAKE) -s -C $(subst .all,,$@) all DEBUG=$(DEBUG) ARCH=$(ARCH) BUILDDIR=$(BUILDDIR)/$(subst .all,,$@) SYSROOTDIR=$(SYSROOTDIR) TOOLROOTDIR=$(TOOLROOTDIR)

$(addsuffix .hostall,$(TOOLS)):
	@echo -e "+-- \e[1;36mBuilding tool: $(subst .hostall,,$@)\e[1;0m"
	@$(MAKE) -s -C $(subst .hostall,,$@) hostall DEBUG=$(DEBUG) ARCH=$(ARCH) BUILDDIR=$(TOOLBUILDDIR)/$(subst .hostall,,$@) SYSROOTDIR=$(SYSROOTDIR) TOOLROOTDIR=$(TOOLROOTDIR)

.PHONY: all toolchain clean image run super superclean
