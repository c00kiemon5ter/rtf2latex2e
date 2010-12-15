CC?=gcc
TAR?=gnutar
RM?=rm -f
MKDIR?=mkdir -p
RMDIR?=rm -rf
PKGMANDIR?=man

#reasonable default set of compiler flags
CFLAGS=-g -Wall -Wno-write-strings

PLATFORM?=-DUNIX   # Mac OS X, Linux, BSD
#PLATFORM?=-DMSDOS # Windows/DOS

#Uncomment for some windows machines (neither needed for djgpp nor for MinGW)
#EXE_SUFFIX=.exe

#Base directory - adapt as needed
# Unix:
PREFIX?=/opt/local
#Uncomment next line for DOS/Windows
#PREFIX_DRIVE=C:
#PREFIX?=$(PREFIX_DRIVE)/PROGRA~1/latex2rtf

#Name of executable binary --- beware of 8.3 restriction under DOS
BINARY_NAME=rtf2latex$(EXE_SUFFIX)

# Location of binary, man, info, and support files - adapt as needed
BIN_INSTALL=$(PREFIX)/bin
MAN_INSTALL=$(PREFIX)/$(PKGMANDIR)/man1
INFO_INSTALL=$(PREFIX)/info
SUPPORT_INSTALL=$(PREFIX)/share/rtf2latex
PREF_INSTALL=$(PREFIX)/share/rtf2latex/pref

# Nothing to change below this line

CFLAGS:=$(CFLAGS) $(PLATFORM) -DPREFDIR=\"$(PREF_INSTALL)\"

LIBS= -lm

R2L_VERSION:="latex2rtf-`grep 'Version' version.h | sed 's/[^"]*"\([^" ]*\).*/\1/'`"

SRCS=src/LaTeX2e-writer.c      src/cole_decode.c         src/figure2eps.c \
     src/cole.c                src/cole_support.c        src/jpeg2eps.c   \
     src/cole_internal.c       src/cole_version.c        src/reader.c     \
     src/cole_encode.c         src/eqn.c                 src/main.c \
     src/tokenscan.c

HDRS=src/cole.h          src/cole_internal.h src/rtf-ctrldef.h   src/rtf.h       \
     src/stdcharnames.h  src/tokenscan.h     src/eqn.h           src/jpeg2eps.h  \
     src/rtf-namedef.h   src/rtf2LaTeX2e.h   src/cole_support.h

PREFS=pref/TeX-map           pref/TeX-map.latin1    pref/cp437.map         pref/r2l-map \
      pref/TeX-map.applemac  pref/ansi-sym          pref/cp850.map         pref/r2l-pref \
      pref/TeX-map.cp1250    pref/applemac.map      pref/mac-sym           pref/rtf-ctrl \
      pref/TeX-map.cp1252    pref/cp1250.map        pref/pc-sym \
      pref/TeX-map.default   pref/cp1252.map        pref/pca-sym \
      pref/TeX-map.german    pref/cp1254.map        pref/r2l-head

DOCS= doc/GPL_license          doc/rtf2LaTeX2eDoc.html  doc/rtf2latex2eSWP.tex   \
     doc/rtfReader.tex         doc/Release-notes.txt    doc/rtf2LaTeX2eDoc.pdf   \
     doc/rtfReader.dvi         doc/rtf2LaTeX2eDoc.dvi   doc/rtf2LaTeX2eDoc.tex   \
     doc/rtfReader.pdf

README= README INSTALL

TEST=  test/Equation.rtf    test/JPEG_Image.rtf  test/table.rtf       test/test.rtf
	
OBJS=src/LaTeX2e-writer.o      src/cole_decode.o         src/figure2eps.o \
     src/cole.o                src/cole_support.o        src/jpeg2eps.o   \
     src/cole_internal.o       src/cole_version.o        src/reader.o     \
     src/cole_encode.o         src/eqn.o                 src/main.o \
     src/tokenscan.o

all : checkdir rtf2latex

rtf2latex: $(OBJS) $(HDRS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS)	$(LIBS) -o $(BINARY_NAME)

check test: rtf2latex
	cd test && $(MAKE) clean
	cd test && $(MAKE)
	cd test && $(MAKE) check

checkdir: $(README) $(SRCS) $(HDRS) $(PREFS) $(TEST) Makefile

clean: checkdir
	-$(RM) $(OBJS) core $(BINARY_NAME)
	-$(RMDIR) tmp

depend: $(SRCS)
	$(CC) -MM $(SRCS) >makefile.depend
	@echo "***** Append makefile.depend to Makefile manually ******"

dist: checkdir releasedate latex2rtf doc $(SRCS) $(HDRS) $(CFGS) $(README) Makefile vms_make.com $(SCRIPTS) $(DOCS) $(TEST)
	$(MAKE) releasedate
	$(MKDIR) $(L2R_VERSION)
	$(MKDIR) $(L2R_VERSION)/cfg
	$(MKDIR) $(L2R_VERSION)/doc
	$(MKDIR) $(L2R_VERSION)/test
	$(MKDIR) $(L2R_VERSION)/scripts
	ln $(SRCS)         $(L2R_VERSION)
	ln $(HDRS)         $(L2R_VERSION)
	ln $(README)       $(L2R_VERSION)
	ln Makefile        $(L2R_VERSION)
	ln vms_make.com    $(L2R_VERSION)
	ln $(CFGS)         $(L2R_VERSION)/cfg
	ln $(DOCS)         $(L2R_VERSION)/doc
	ln $(SCRIPTS)      $(L2R_VERSION)/scripts
	ln $(TEST)         $(L2R_VERSION)/test
	$(TAR) cvfz $(L2R_VERSION).tar.gz $(L2R_VERSION)
	$(RMDIR) $(L2R_VERSION)

uptodate:
	perl -pi.bak -e '$$date=scalar localtime; s/\(.*/($$date)";/' version.h
	$(RM) version.h.bak

releasedate:
	perl -pi.bak -e '$$date=scalar localtime; s/\(.*/(released $$date)";/; s/d ..../d /;s/\d\d:\d\d:\d\d //;' version.h
	$(RM) version.h.bak

doc: doc/latex2rtf.texi doc/Makefile
	cd doc && $(MAKE) -k

install: latex2rtf doc/latex2rtf.1 $(CFGS) scripts/latex2png
	cd doc && $(MAKE)
	$(MKDIR) $(BIN_INSTALL)
	$(MKDIR) $(MAN_INSTALL)
	$(MKDIR) $(CFG_INSTALL)
	cp $(BINARY_NAME)     $(BIN_INSTALL)
	cp scripts/latex2png  $(BIN_INSTALL)
	cp doc/latex2rtf.1    $(MAN_INSTALL)
	cp doc/latex2png.1    $(MAN_INSTALL)
	cp $(CFGS)            $(CFG_INSTALL)
	cp doc/latex2rtf.html $(SUPPORT_INSTALL)
	cp doc/latex2rtf.pdf  $(SUPPORT_INSTALL)
	cp doc/latex2rtf.txt  $(SUPPORT_INSTALL)
	@echo
	@echo "rtf2latex has been installed in $(INSTALL_DIR)"
	@echo
	@echo "Please set the environment variable RTF2LATEX2E_DIR to $(INSTALL_DIR)."
	@echo

install-info: doc/latex2rtf.info
	$(MKDIR) $(INFO_INSTALL)
	cp doc/latex2rtf.info $(INFO_INSTALL)
	install-info --info-dir=$(INFO_INSTALL) doc/latex2rtf.info

realclean: checkdir clean
	$(RM) makefile.depend $(L2R_VERSION).tar.gz
	cd doc && $(MAKE) clean
	cd test && $(MAKE) clean

appleclean:
	sudo xattr -r -d com.apple.FinderInfo ../trunk
	sudo xattr -r -d com.apple.TextEncoding ../trunk
	
splint: 
	splint -weak $(SRCS) $(HDRS)
	
.PHONY: all check checkdir clean depend dist doc install install_info realclean latex2rtf uptodate releasedate splint fullcheck

# created using "make depend"
cole.o: src/cole.c
cole_decode.o: src/cole_decode.c src/cole.h src/cole_support.h src/cole_internal.h
cole_encode.o: src/cole_encode.c src/cole.h src/cole_support.h src/cole_internal.h
cole_internal.o: src/cole_internal.c src/cole_internal.h src/cole_support.h
cole_support.o: src/cole_support.c
cole_version.o: src/cole_version.c
LaTeX2e-writer.o: src/LaTeX2e-writer.c src/rtf.h src/rtf-ctrldef.h src/rtf-namedef.h src/tokenscan.h src/cole.h src/rtf2LaTeX2e.h src/eqn.h
figure2eps.o: src/figure2eps.c
jpeg2eps.o: src/jpeg2eps.c src/rtf.h src/rtf-ctrldef.h src/rtf-namedef.h src/rtf2LaTeX2e.h src/tokenscan.h src/jpeg2eps.h
reader.o: src/reader.c src/tokenscan.h src/rtf.h src/rtf-ctrldef.h src/rtf-namedef.h src/rtf2LaTeX2e.h src/stdcharnames.h
eqn.o: src/eqn.c src/rtf.h src/rtf-ctrldef.h src/rtf-namedef.h src/rtf2LaTeX2e.h src/eqn.h
main.o: src/main.c src/rtf.h src/rtf-ctrldef.h src/rtf-namedef.h src/rtf2LaTeX2e.h
tokenscan.o: src/tokenscan.c src/tokenscan.h
