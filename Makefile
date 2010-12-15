VERSION = 1.2.0

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
SUPPORT_INSTALL=$(PREFIX)/share/rtf2latex

# Nothing to change below this line

CFLAGS:=$(CFLAGS) $(PLATFORM) 

LIBS= 

SRCS=src/writer.c              src/cole_decode.c         src/figure2eps.c \
     src/cole.c                src/cole_support.c        src/jpeg2eps.c   \
     src/cole_internal.c       src/cole_version.c        src/reader.c     \
     src/cole_encode.c         src/eqn.c                 src/main.c       \
     src/mygetopt.c

HDRS=src/cole.h        src/cole_internal.h   src/rtf.h           src/eqn.h   \
     src/jpeg2eps.h    src/rtf2LaTeX2e.h     src/cole_support.h  src/mygetopt.h

RTFPREP_SRCS = src/rtfprep/Makefile     src/rtfprep/rtf-controls  src/rtfprep/rtfprep.c  src/rtfprep/standard-names \
               src/rtfprep/tokenscan.c  src/rtfprep/tokenscan.h  

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

TEST = test/Makefile      test/arch.rtf      test/fig-jpeg.rtf  test/multiline.rtf test/rtf.rtf \
       test/arch-mac.rtf  test/equation.rtf  test/mapping.rtf   test/rtf-misc.rtf  test/table.rtf \
       test/test.rtf

README= README INSTALL

TEST = test/Makefile      test/arch.rtf      test/fig-jpeg.rtf  test/multiline.rtf test/rtf.rtf \
       test/arch-mac.rtf  test/equation.rtf  test/mapping.rtf   test/rtf-misc.rtf  test/table.rtf
	
OBJS=src/writer.o              src/cole_decode.o         src/figure2eps.o \
     src/cole.o                src/cole_support.o        src/jpeg2eps.o   \
     src/cole_internal.o       src/cole_version.o        src/reader.o     \
     src/cole_encode.o         src/eqn.o                 src/main.o \
     src/rtfprep/tokenscan.o   src/mygetopt.o

all : checkdir rtf2latex

rtf2latex: $(OBJS) $(HDRS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS)	$(LIBS) -o $(BINARY_NAME)

src/main.o: Makefile src/main.c
	$(CC) $(CFLAGS) -DLIBDIR=\"$(SUPPORT_INSTALL)\" -c src/main.c -o src/main.o

src/rtfprep/rtf-ctrldef.h  src/rtfprep/rtf-namedef.h  src/rtfprep/stdcharnames.h src/rtfprep/tokenscan.o:
	cd src/rtfprep && make

check test: rtf2latex
	cd test && $(MAKE)

checkdir: $(README) $(SRCS) $(HDRS) $(PREFS) $(TEST) Makefile

depend: $(SRCS)
	$(CC) -MM $(SRCS) >makefile.depend
	@echo "***** Append makefile.depend to Makefile manually ******"

dist: checkdir doc $(SRCS) $(RTFPREP_SRC) $(HDRS) $(README) $(PREF) $(DOCS) $(TEST) Makefile
	$(MKDIR)           rtf2latex2e-$(VERSION)
	$(MKDIR)           rtf2latex2e-$(VERSION)/pref
	$(MKDIR)           rtf2latex2e-$(VERSION)/doc
	$(MKDIR)           rtf2latex2e-$(VERSION)/test
	$(MKDIR)           rtf2latex2e-$(VERSION)/src
	$(MKDIR)           rtf2latex2e-$(VERSION)/src/rtfprep
	ln $(README)       rtf2latex2e-$(VERSION)
	ln Makefile        rtf2latex2e-$(VERSION)
	ln $(SRCS)         rtf2latex2e-$(VERSION)/src
	ln $(HDRS)         rtf2latex2e-$(VERSION)/src
	ln $(RTFPREP_SRCS) rtf2latex2e-$(VERSION)/src/rtfprep
	ln $(PREF)         rtf2latex2e-$(VERSION)/pref
	ln $(DOCS)         rtf2latex2e-$(VERSION)/doc
	ln $(TEST)         rtf2latex2e-$(VERSION)/test
	tar cvf - rtf2latex2e-$(VERSION) | gzip > rtf2latex2e-$(VERSION).tar.gz
	rm -rf rtf2latex2e-$(VERSION)
	
doc/rtf2LaTeX2eDoc.html: doc
doc/rtf2LaTeX2eDoc.pdf : doc

doc: doc/rtf2LaTeX2eDoc.tex doc/Makefile
	cd doc && $(MAKE) -k

install: rtf2latex2e $(PREF) doc/rtf2LaTeX2eDoc.html doc/rtf2LaTeX2eDoc.pdf
	$(MKDIR)                   $(BIN_INSTALL)
	$(MKDIR)                   $(SUPPORT_INSTALL)
	cp $(BINARY_NAME)          $(BIN_INSTALL)
	cp $(PREF)                 $(SUPPORT_INSTALL)
	cp doc/rtf2LaTeX2eDoc.html $(SUPPORT_INSTALL)
	cp doc/rtf2LaTeX2eDoc.pdf  $(SUPPORT_INSTALL)
	@echo "******************************************************************"
	@echo "*** rtf2latex2e successfully installed as \"$(BINARY_NAME)\""
	@echo "*** in directory \"$(BIN_INSTALL)\""
	@echo "***"
	@echo "*** rtf2latex2e was compiled to search for its configuration files in"
	@echo "***           \"$(SUPPORT_INSTALL)\" "
	@echo "***"
	@echo "*** If the configuration files are moved then either"
	@echo "***   1) set the environment variable RTFPATH to this new location, or"
	@echo "***   2) use the command line option -P /path/to/prefs, or"
	@echo "***   3) edit the Makefile and recompile"
	@echo "******************************************************************"

clean: 
	rm -f $(OBJS) $(BINARY_NAME)
	cd test   && make clean
	cd doc    && make clean
	cd src/rtfprep && make clean
	
realclean: checkdir clean
	rm -f makefile.depend rtf2latex2e-$(VERSION).tar.gz
	rm -f src/rtf-ctrldef.h  src/rtf-namedef.h  src/stdcharnames.h
	cd test   && make realclean
	cd doc    && make realclean
	cd src/rtfprep && make realclean
	
appleclean:
	sudo xattr -r -d com.apple.FinderInfo ../trunk
	sudo xattr -r -d com.apple.TextEncoding ../trunk
	
splint: 
	splint -weak $(SRCS) $(HDRS)
	
.PHONY: all check checkdir clean depend dist doc install realclean test

# created using "make depend"
src/cole.o:          src/cole.c
src/cole_decode.o:   src/cole_decode.c src/cole.h src/cole_support.h src/cole_internal.h
src/cole_encode.o:   src/cole_encode.c src/cole.h src/cole_support.h src/cole_internal.h
src/cole_internal.o: src/cole_internal.c src/cole_internal.h src/cole_support.h
src/cole_support.o:  src/cole_support.c
src/cole_version.o:  src/cole_version.c
src/eqn.o:           src/eqn.c src/rtf.h src/rtf2LaTeX2e.h src/eqn.h
src/figure2eps.o:    src/figure2eps.c
src/jpeg2eps.o:      src/jpeg2eps.c src/rtf.h src/rtf2LaTeX2e.h src/rtfprep/tokenscan.h src/jpeg2eps.h
src/main.o:          src/main.c src/rtf.h src/rtf2LaTeX2e.h src/mygetopt.h
src/reader.o:        src/reader.c src/rtfprep/tokenscan.h src/rtf.h src/rtf2LaTeX2e.h src/rtfprep/stdcharnames.h
src/rtf.h:           src/rtfprep/rtf-ctrldef.h src/rtfprep/rtf-namedef.h
src/writer.o:        src/writer.c src/rtf.h src/rtfprep/tokenscan.h src/cole.h src/rtf2LaTeX2e.h src/eqn.h
