.PHONY: hostall

hostall: $(BUILDDIR)/build
	@echo -e "    +-- \e[1;35mBuilding gcc...\e[1;0m"
	@cd $(BUILDDIR)/build 
	@$(MAKE) -C $(BUILDDIR)/build all-gcc all-target-libgcc >/dev/null
	@$(MAKE) -C $(BUILDDIR)/build install-gcc install-target-libgcc DESTDIR=$(TOOLROOTDIR) >/dev/null

$(BUILDDIR)/build: $(BUILDDIR)/src
	@echo -e "    +-- \e[1;35mConfiguring gcc...\e[1;0m"
	@mkdir -p $(BUILDDIR)/build
	@cd $(BUILDDIR)/build && ../src/configure --target=x86_64-duckos --prefix="/usr" --with-sysroot=$(TOOLROOTDIR) --enable-languages=c,c++ >/dev/null

$(BUILDDIR)/src: gcc-11.2.0-duckos.tar.gz
	@echo -e "    +-- \e[1;35mExtracting gcc...\e[1;0m"
	@mkdir -p $(BUILDDIR)/src
	@tar -xf "gcc-11.2.0-duckos.tar.gz" -C "$(BUILDDIR)/src" --strip-components=1

gcc-11.2.0-duckos.tar.gz:
	@cat gcc-11.2.0-duckos.tar.gz.part.* > gcc-11.2.0-duckos.tar.gz
