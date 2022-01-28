DEBUG ?= false

.PHONY: all

all: 
	mkdir -p $(BUILDDIR)
	cp $(SYSROOTDIR)/init $(BUILDDIR)/init
	cd $(BUILDDIR); tar -cf $(SYSROOTDIR)/initrd.tar init
