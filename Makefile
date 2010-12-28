VERSION = 1-3-0

CC?=gcc
TAR?=gnutar
RM?=rm -f
MKDIR?=mkdir -p
RMDIR?=rm -rf

#reasonable default set of compiler flags
CFLAGS=-g -Wall -Wno-write-strings

PLATFORM?=-DUNIX   # Mac OS X, Linux, BSD
#PLATFORM?=-DMSDOS # Windows/DOS

#Base directory - adapt as needed
# Unix:
PREFIX?=/opt/local
#Uncomment next line for DOS/Windows
#PREFIX_DRIVE=C:
#PREFIX?=$(PREFIX_DRIVE)/PROGRA~1/latex2rtf

#Name of executable binary --- beware of 8.3 restriction under DOS
BINARY_NAME=rtf2latex2e

# Location of binary, man, info, and support files - adapt as needed
BIN_INSTALL    =$(PREFIX)/bin
SUPPORT_INSTALL=$(PREFIX)/share/rtf2latex2e

# Nothing to change below this line

CFLAGS:=$(CFLAGS) $(PLATFORM) 
# CFLAGS:=$(CFLAGS) $(PLATFORM) -DCOLE_VERBOSE

LIBS= 

SRCS         = src/cole.c                 src/cole_decode.c         src/cole_encode.c       \
               src/cole_internal.c        src/cole_support.c        src/cole_version.c      \
               src/eqn.c                  src/figure2eps.c          src/jpeg2eps.c          \
               src/main.c                 src/mygetopt.c            src/reader.c            \
               src/writer.c

HDRS         = src/cole.h                 src/cole_internal.h       src/rtf.h               \
               src/eqn.h                  src/jpeg2eps.h            src/rtf2latex2e.h       \
               src/cole_support.h         src/mygetopt.h

RTFPREP_SRCS = src/rtfprep/Makefile       src/rtfprep/rtf-controls  src/rtfprep/rtfprep.c   \
               src/rtfprep/standard-names src/rtfprep/tokenscan.c   src/rtfprep/tokenscan.h  

PREFS        = pref/TeX-map               pref/TeX-map.latin1       pref/cp437.map          \
               pref/TeX-map.applemac      pref/ansi-sym             pref/cp850.map          \
               pref/TeX-map.cp1250        pref/applemac.map         pref/mac-sym            \
               pref/TeX-map.cp1252        pref/cp1250.map           pref/pc-sym             \
               pref/TeX-map.default       pref/cp1252.map           pref/pca-sym            \
               pref/TeX-map.german        pref/cp1254.map           pref/r2l-head           \
               pref/r2l-map               pref/r2l-pref             pref/rtf-ctrl

DOCS         = doc/GPL_license            doc/Release-notes.txt     README                  \
               doc/rtf2latex2eSWP.tex     doc/rtfReader.tex         doc/rtf2latex2eDoc.tex
               
PDFS         = doc/rtf2latex2eSWP.pdf     doc/rtfReader.pdf         doc/rtf2latex2eDoc.pdf  

TEST         = test/Makefile              test/arch.rtf             test/arch-mac.rtf       \
               test/equation.rtf          test/fig-jpeg.rtf         test/multiline.rtf      \
               test/mapping.rtf           test/rtf-misc.rtf         test/rtf.rtf            \
               test/table.rtf             test/test.rtf             test/moreEqns.rtf

OBJS         = src/cole.o                 src/cole_decode.o         src/cole_encode.o       \
               src/cole_internal.o        src/cole_support.o        src/cole_version.o      \
               src/eqn.o                  src/figure2eps.o          src/jpeg2eps.o          \
               src/main.o                 src/mygetopt.o            src/reader.o            \
               src/rtfprep/tokenscan.o    src/writer.o

all : checkdir rtfprep rtf2latex2e

rtfprep:
	cd src/rtfprep && $(MAKE)
	cp src/rtfprep/rtf-ctrl pref/rtf-ctrl

rtf2latex2e: $(OBJS) $(HDRS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS)	$(LIBS) -o $(BINARY_NAME)

src/main.o: Makefile src/main.c
	$(CC) $(CFLAGS) -DLIBDIR=\"$(SUPPORT_INSTALL)\" -c src/main.c -o src/main.o

doc : doc/rtf2latex2eSWP.tex doc/rtfReader.tex doc/rtf2latex2eDoc.tex
	cd doc && $(MAKE)

check test: rtf2latex2e
	cd test && $(MAKE)

checkdir: $(SRCS) $(HDRS) $(PREFS) $(TEST) $(DOCS) Makefile

depend: $(SRCS)
	$(CC) -MM $(SRCS) >makefile.depend
	@echo "***** Append makefile.depend to Makefile manually ******"

dist: checkdir doc $(SRCS) $(RTFPREP_SRC) $(HDRS) $(README) $(PREFS) $(TEST) $(DOCS) Makefile
	make doc
	$(MKDIR)           rtf2latex2e-$(VERSION)
	$(MKDIR)           rtf2latex2e-$(VERSION)/pref
	$(MKDIR)           rtf2latex2e-$(VERSION)/doc
	$(MKDIR)           rtf2latex2e-$(VERSION)/test
	$(MKDIR)           rtf2latex2e-$(VERSION)/src
	$(MKDIR)           rtf2latex2e-$(VERSION)/src/rtfprep
	ln README          rtf2latex2e-$(VERSION)
	ln Makefile        rtf2latex2e-$(VERSION)
	ln $(SRCS)         rtf2latex2e-$(VERSION)/src
	ln $(HDRS)         rtf2latex2e-$(VERSION)/src
	ln $(RTFPREP_SRCS) rtf2latex2e-$(VERSION)/src/rtfprep
	ln $(PREFS)        rtf2latex2e-$(VERSION)/pref
	ln $(DOCS)         rtf2latex2e-$(VERSION)/doc
	ln $(PDFS)         rtf2latex2e-$(VERSION)/doc
	ln $(TEST)         rtf2latex2e-$(VERSION)/test
#	tar cvf - rtf2latex2e-$(VERSION) | gzip > rtf2latex2e-$(VERSION).tar.gz
	zip -r rtf2latex2e-$(VERSION) rtf2latex2e-$(VERSION)
	rm -rf rtf2latex2e-$(VERSION)
	
install:
	$(MAKE) rtfprep
	$(MAKE)
	$(MKDIR)                   $(BIN_INSTALL)
	$(MKDIR)                   $(SUPPORT_INSTALL)
	cp $(BINARY_NAME)          $(BIN_INSTALL)
	cp $(PREFS)                $(SUPPORT_INSTALL)
	cp doc/rtf2latex2eDoc.pdf  $(SUPPORT_INSTALL)
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
	
.PHONY: all check checkdir clean depend dist doc install realclean rtfprep test

# created using "make depend"
cole.o:          src/cole.c src/cole.h src/cole_internal.h src/cole_support.h
cole_decode.o:   src/cole_decode.c src/cole.h src/cole_support.h src/cole_internal.h
cole_encode.o:   src/cole_encode.c src/cole.h src/cole_support.h src/cole_internal.h
cole_internal.o: src/cole_internal.c src/cole_internal.h src/cole_support.h
cole_support.o:  src/cole_support.c src/cole_support.h
cole_version.o:  src/cole_version.c
eqn.o:           src/eqn.c src/rtf.h src/rtfprep/rtf-ctrldef.h src/rtfprep/rtf-namedef.h \
                 src/rtf2latex2e.h src/cole_support.h src/eqn.h src/eqn_support.h
figure2eps.o: src/figure2eps.c
jpeg2eps.o: src/jpeg2eps.c src/rtf.h src/rtfprep/rtf-ctrldef.h src/rtfprep/rtf-namedef.h \
            src/rtfprep/tokenscan.h src/rtf2latex2e.h src/jpeg2eps.h
main.o: src/main.c src/rtf.h src/rtfprep/rtf-ctrldef.h src/rtfprep/rtf-namedef.h src/mygetopt.h src/rtf2latex2e.h
mygetopt.o: src/mygetopt.c src/mygetopt.h
reader.o: src/reader.c src/rtfprep/tokenscan.h src/rtf.h src/rtfprep/rtf-ctrldef.h \
          src/rtfprep/rtf-namedef.h src/rtf2latex2e.h src/rtfprep/stdcharnames.h
writer.o: src/writer.c src/rtf.h src/rtfprep/rtf-ctrldef.h src/rtfprep/rtf-namedef.h \
          src/rtfprep/tokenscan.h src/cole.h src/cole_support.h src/rtf2latex2e.h src/eqn.h
