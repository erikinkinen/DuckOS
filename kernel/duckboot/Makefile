DEBUG ?= false

.PHONY: all clean

all: $(BUILDDIR)/src
	@$(MAKE) -s -C $(BUILDDIR) -f boot.mk all
	@mkdir -p $(SYSROOTDIR)/efi/boot/
	@cp -u $(CURDIR)/prebuilt/zap-light16.psf $(SYSROOTDIR)/font.psf
	@cp -u $(BUILDDIR)/main.efi $(SYSROOTDIR)/efi/boot/bootx64.efi

$(BUILDDIR)/src:
	@mkdir -p $(BUILDDIR)
	@cp -r $(CURDIR)/* $(BUILDDIR)