DEBUG ?= false

CC = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-gcc
AS = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-as
AR = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-ar
CXX = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-g++
LD = $(TOOLROOTDIR)/usr/bin/$(ARCH)-duckos-ld

LDFLAGS = -static

OBJS += \
		init.o

ABSOBJS = $(addprefix $(BUILDDIR)/,$(OBJS))

.PHONY: all clean

all: $(BUILDDIR)/init
	@mkdir -p $(SYSROOTDIR)/
	@cp -u -r $(BUILDDIR)/init $(SYSROOTDIR)/

$(BUILDDIR)/init: $(ABSOBJS)
	@echo -e "    +-- \e[1;35mLinking DuckInit...\e[1;0m"
	@mkdir -p $(dir $@)
	@$(CC) $(LDFLAGS) -o $@ $(ABSOBJS)

$(BUILDDIR)/%.o: $(CURDIR)/%.c
	@echo -e "    +-- \e[1;32mCompiling $(subst $(CURDIR)/,,$<)...\e[1;0m"
	@mkdir -p $(dir $@)
	@$(CC) -o $@ -c $<

$(BUILDDIR)/%.o: $(CURDIR)/%.S
	@echo -e "    +-- \e[1;32mAssembing $(subst $(CURDIR)/,,$<)...\e[1;0m"
	@mkdir -p $(dir $@)
	@$(AS) $< -o $@