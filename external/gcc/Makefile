.PHONY: hostall

hostall: $(BUILDDIR)/build_$(ARCH)
	@echo -e "    +-- \e[1;35mBuilding gcc...\e[1;0m"
	@cd $(BUILDDIR)/build_$(ARCH)
	@$(MAKE) -C $(BUILDDIR)/build_$(ARCH) all-gcc all-target-libgcc >/dev/null
	@$(MAKE) -C $(BUILDDIR)/build_$(ARCH) install-gcc install-target-libgcc DESTDIR=$(TOOLROOTDIR) >/dev/null
	@echo -e "    +-- \e[1;35mInstalling gcc...\e[1;0m"
	@sudo bash -c "$(MAKE) -C $(BUILDDIR)/build_$(ARCH) install-gcc install-target-libgcc DESTDIR=/ >/dev/null"

$(BUILDDIR)/build_$(ARCH): $(BUILDDIR)/src
	@echo -e "    +-- \e[1;35mConfiguring gcc...\e[1;0m"
	@mkdir -p $(BUILDDIR)/build_$(ARCH)
	@cd $(BUILDDIR)/build_$(ARCH) && ../src/configure --target=$(ARCH)-duckos --prefix="/usr" --with-sysroot=$(TOOLROOTDIR) --enable-languages=c,c++ >/dev/null

$(BUILDDIR)/src: gcc-11.2.0-duckos.tar.gz
	@echo -e "    +-- \e[1;35mExtracting gcc...\e[1;0m"
	@mkdir -p $(BUILDDIR)/src
	@tar -xf "gcc-11.2.0-duckos.tar.gz" -C "$(BUILDDIR)/src" --strip-components=1

gcc-11.2.0-duckos.tar.gz:
	@cat gcc-11.2.0-duckos.tar.gz.part.* > gcc-11.2.0-duckos.tar.gz
