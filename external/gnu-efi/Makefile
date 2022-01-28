DEBUG ?= false

.PHONY: all clean

all: $(BUILDDIR)
	$(MAKE) -C $(BUILDDIR) all >/dev/null

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)
	@[[ -f gnu-efi-3.0.14.tar.bz2 ]] || wget https://download.sourceforge.net/gnu-efi/gnu-efi-3.0.14.tar.bz2 -O gnu-efi-3.0.14.tar.bz2
	@tar -xf gnu-efi-3.0.14.tar.bz2 -C "$(BUILDDIR)/" --strip-components=1