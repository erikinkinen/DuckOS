DEBUG ?= false

CC = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-gcc
AS = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-as
AR = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-ar
CXX = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-g++
LD = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-ld

CFLAGS += -I$(CURDIR)/include -I$(CURDIR)/include/arch/$(ARCH)

OBJS += \
		stdio/printf.o \
		stdio/puts.o \
		stdio/vfprintf.o \
		string/strlen.o

ARCHOBJS += \
		crt0.o \
		crti.o \
		crtn.o

OBJS += $(addprefix arch/$(ARCH)/,$(ARCHOBJS))

ABSOBJS = $(addprefix $(BUILDDIR)/,$(OBJS))

.PHONY: all clean

all: $(BUILDDIR)/libc.a
	@mkdir -p $(TOOLROOTDIR)/usr/lib
	@cp -u -r $(BUILDDIR)/libc.a $(TOOLROOTDIR)/usr/lib
	@cp -u -r $(BUILDDIR)/arch/$(ARCH)/crt*.o $(TOOLROOTDIR)/usr/lib

hostall:
	@mkdir -p $(TOOLROOTDIR)/usr/include
	@cp -u -r $(CURDIR)/include/* $(TOOLROOTDIR)/usr/include/

$(BUILDDIR)/libc.a: $(ABSOBJS)
	@echo -e "    +-- \e[1;35mLinking DuckLibC...\e[1;0m"
	@mkdir -p $(dir $@)
	@$(AR) rcs $@ $(ABSOBJS)

$(BUILDDIR)/%.o: $(CURDIR)/%.c
	@echo -e "    +-- \e[1;32mCompiling $(subst $(CURDIR)/,,$<)...\e[1;0m"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -o $@ -c $<

$(BUILDDIR)/%.o: $(CURDIR)/%.S
	@echo -e "    +-- \e[1;32mAssembing $(subst $(CURDIR)/,,$<)...\e[1;0m"
	@mkdir -p $(dir $@)
	@$(AS) $< -o $@