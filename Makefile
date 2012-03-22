
PATH_INCLUDE	:= /usr/include
PATH_INSTALL	:= /usr/lib

LIBNAME	:= libbttc
VERSION  := 2.0

OBJS		:= bttc.o

CFLAGS	:= -O3 -fPIC -W -Wall -Wextra -Werror -pedantic -ansi -Wconversion -Wunused -Wshadow -Wpointer-arith -Wmissing-prototypes -Winline -Wcast-align -Wmissing-declarations -Wredundant-decls -Wno-long-long -Wcast-align -Werror
#CFLAGS	:= -g -DDQ_CHECK -fPIC -W -Wall -Wextra -Werror -pedantic -ansi
LDFLAGS	:= -lm


.PHONY: all lib install uninstall clean docs help


all: libbttc bttc_m.mex

help:
	@echo "Valid targets are:"
	@echo "          all - Makes the library and tests it"
	@echo "        libdq - Makes the libdq library"
	@echo "      install - Installs the library"
	@echo "    uninstall - Uninstalls the library"
	@echo "         docs - Makes the documentation"
	@echo "        clean - Cleans up the build system"

$(LIBNAME): $(LIBNAME).a $(LIBNAME).so

$(LIBNAME).a: $(OBJS)
	$(AR) rcs $(LIBNAME).a $(OBJS)

$(LIBNAME).so: $(OBJS)
	$(CC) -lm -shared -Wl,-soname,$(LIBNAME).so -o $(LIBNAME).so.$(VERSION) $(OBJS)
	ln -sf $(LIBNAME).so.$(VERSION) $(LIBNAME).so

bttc_m.mex: bttc_m.c bttc.c bttc.h
	mkoctfile --mex bttc_m.c bttc.c

install: all
	install -m 644 $(LIBNAME).a $(PATH_INSTALL)
	test -d $(PATH_INCLUDE) || mkdir $(PATH_INCLUDE)
	cp bttc.h      $(PATH_INCLUDE)/bttc.h
	cp $(LIBNAME).so.$(VERSION) $(PATH_INSTALL)
	(cd $(PATH_INSTALL); ln -sf $(PATH_INSTALL)/$(LIBNAME).so.$(VERSION) $(LIBNAME).so)
	ldconfig

uninstall:
	$(RM) $(PATH_INCLUDE)/bttc.h
	$(RM) $(PATH_INSTALL)/$(LIBNAME).so.$(VERSION)
	$(RM) $(PATH_INSTALL)/$(LIBNAME).so
	$(RM) $(PATH_INSTALL)/$(LIBNAME).a

docs:
	doxygen
	$(MAKE) -C docs/latex
	cp docs/latex/refman.pdf $(LIBNAME).pdf

clean:
	$(RM) $(OBJS) $(LIBNAME).a $(LIBNAME).so $(LIBNAME).so.$(VERSION)
	$(RM) bttc_m.mex


