all: checkmakefiles
	@cd src && $(MAKE)

clean: checkmakefiles
	@cd src && $(MAKE) clean

cleanall: checkmakefiles
	@cd src && $(MAKE) MODE=release clean
	@cd src && $(MAKE) MODE=debug clean
	@rm -f src/Makefile

INET_PROJ=../inet
makefiles:
	@cd src && opp_makemake -o flora -f --deep -r -KINET_PROJ=../inet -I$(INET_PROJ)/src/ -L$$\(INET_PROJ\)/out/$$\(CONFIGNAME\)/src -lINET

checkmakefiles:
	@if [ ! -f src/Makefile ]; then \
	echo; \
	echo '======================================================================='; \
	echo 'src/Makefile does not exist. Please use "make makefiles" to generate it!'; \
	echo '======================================================================='; \
	echo; \
	exit 1; \
	fi
