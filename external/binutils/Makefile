.PHONY: hostall

hostall: $(BUILDDIR)/build_$(ARCH)
	@echo -e "    +-- \e[1;35mBuilding binutils...\e[1;0m"
	@cd $(BUILDDIR)/build_$(ARCH)
	@$(MAKE) -C $(BUILDDIR)/build_$(ARCH) all >/dev/null
	@$(MAKE) -C $(BUILDDIR)/build_$(ARCH) install DESTDIR=$(TOOLROOTDIR) >/dev/null
	@echo -e "    +-- \e[1;35mInstalling binutils...\e[1;0m"
	@sudo bash -c "$(MAKE) -C $(BUILDDIR)/build_$(ARCH) install DESTDIR=/ >/dev/null"

$(BUILDDIR)/build_$(ARCH): $(BUILDDIR)/src
	@echo -e "    +-- \e[1;35mConfiguring binutils...\e[1;0m"
	@mkdir -p $(BUILDDIR)/build_$(ARCH)
	@cd $(BUILDDIR)/build_$(ARCH) && ../src/configure --target=$(ARCH)-duckos --prefix="/usr" --with-sysroot=$(TOOLROOTDIR) --disable-werror >/dev/null

$(BUILDDIR)/src:
	@echo -e "    +-- \e[1;35mExtracting binutils...\e[1;0m"
	@mkdir -p $(BUILDDIR)/src
	@tar -xf "binutils-2.37-duckos.tar.gz" -C "$(BUILDDIR)/src" --strip-components=1
