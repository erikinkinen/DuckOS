.PHONY: hostall

hostall: $(BUILDDIR)/build
	@echo -e "    +-- \e[1;35mBuilding binutils...\e[1;0m"
	@cd $(BUILDDIR)/build 
	@$(MAKE) -C $(BUILDDIR)/build all >/dev/null
	@$(MAKE) -C $(BUILDDIR)/build install DESTDIR=$(TOOLROOTDIR) >/dev/null

$(BUILDDIR)/build: $(BUILDDIR)/src
	@echo -e "    +-- \e[1;35mConfiguring binutils...\e[1;0m"
	@mkdir -p $(BUILDDIR)/build
	@cd $(BUILDDIR)/build && ../src/configure --target=x86_64-duckos --prefix="/usr" --with-sysroot=$(TOOLROOTDIR) --disable-werror >/dev/null

$(BUILDDIR)/src:
	@echo -e "    +-- \e[1;35mExtracting binutils...\e[1;0m"
	@mkdir -p $(BUILDDIR)/src
	@tar -xf "binutils-2.37-duckos.tar.gz" -C "$(BUILDDIR)/src" --strip-components=1
