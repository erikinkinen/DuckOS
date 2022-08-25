DEBUG ?= false

CC = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-gcc
AS = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-as
AR = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-ar
CXX = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-g++
LD = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-ld

CFLAGS = -ffreestanding -fshort-wchar -fno-stack-protector -fno-exceptions -fno-rtti \
		 -mgeneral-regs-only -I$(CURDIR)/include -I$(CURDIR)/include/arch/$(ARCH) -DARCH=$(ARCH)
LDFLAGS = -T $(CURDIR)/kernel.$(ARCH).ld -static -Bsymbolic -nostdlib

ifeq ($(ARCH), x86_64)
CFLAGS += -mno-red-zone
else
$(error Architecture specified is not supported)
endif

OBJS += \
		main.o \
		fs/devfs.o \
		fs/fsmanager.o \
		fs/initrd.o \
		memory/heap.o \
		memory/memory.o \
		memory/pageframe.o \
		utils.o \
		utils/bitmap.o \
		utils/logger.o

ifeq ($(ARCH), x86_64)
ARCHOBJS += \
		drivers/pic.o \
		drivers/pit.o \
		interrupts/idt.o \
		interrupts/idt_asm.o \
		memory/gdt.o \
		memory/gdt_asm.o \
		memory/pagetable.o
endif

ARCHOBJS += \
		entry_asm.o \
		arch.o \
		drivers/serial.o \
		task.o

OBJS += $(addprefix arch/$(ARCH)/,$(ARCHOBJS))

ABSOBJS = $(addprefix $(BUILDDIR)/,$(OBJS))

.PHONY: all clean

all: $(BUILDDIR)/kernel.duck
	@cp -u $(BUILDDIR)/kernel.duck $(SYSROOTDIR)/

$(BUILDDIR)/kernel.duck: $(ABSOBJS)
	@echo -e "    +-- \e[1;35mLinking DuckKernel...\e[1;0m"
	@mkdir -p $(dir $@)
	@$(LD) $(LDFLAGS) -o $@ $(ABSOBJS)

$(BUILDDIR)/%.o: $(CURDIR)/%.cpp $(CURDIR)/config.h
	@echo -e "    +-- \e[1;32mCompiling $(subst $(CURDIR)/,,$<)...\e[1;0m"
	@mkdir -p $(dir $@)
	@$(CXX) $(CFLAGS) -o $@ -c $< --include config.h

$(BUILDDIR)/%_asm.o: $(CURDIR)/%.asm
	@echo -e "    +-- \e[1;32mAssembing $(subst $(CURDIR)/,,$<)...\e[1;0m"
	@mkdir -p $(dir $@)
	@$(AS) $< -o $@

$(CURDIR)/config.h:
	@echo -e "    +-- \e[1;32mUsing default configuration as \"config.h\" was not found...\e[1;0m"
	@cp $(CURDIR)/config.def.h $(CURDIR)/config.h