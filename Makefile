VERSION = 2-2-0

CC:=gcc

#reasonable default set of compiler flags while developing
#CFLAGS = -g -D_FORTIFY_SOURCE=2 -Wall -Waggregate-return -Wmissing-declarations -Wmissing-prototypes -Wredundant-decls -Wshadow -Wstrict-prototypes -Wformat=2

#PLATFORM?=-DUNIX   # Mac OS X, Linux, BSD
PLATFORM?=-DMSWIN  # Windows
#PLATFORM?=-DMSDOS  # DOS - emf2pdf.bat does not work under command.com

#Base directory - adapt as needed
# Unix:
#prefix?=/usr/local
#exec_prefix?=$(prefix)

#Uncomment next 2 lines for Windows
prefix_DRIVE=C:
prefix?=$(prefix_DRIVE)/Progra~1/rtf2latex2e

BINARY_NAME=rtf2latex2e

# Location of binary, man, info, and support files - adapt as needed
#package-name = rtf2latex2e
#bindir      ?= $(exec_prefix)/bin
#datarootdir ?= $(prefix)/share
#datadir     ?= $(datarootdir)
#docdir      ?= $(datarootdir)/doc/$(package-name)
#pdfdir      ?= $(docdir)
#mandir      ?= $(datarootdir)/man
#prefsdir    ?= $(datadir)/$(package-name)

#Uncomment next 4 lines for Windows:
bindir    =
datadir   = $(prefix)/pref
prefsdir  = $(datadir)

# Uncomment to get debugging information about OLE translation
#CFLAGS:=$(CFLAGS) -DCOLE_VERBOSE

# Nothing to change below this line

SRCS         = src/cole.c                 src/cole_decode.c          src/cole_support.c      \
               src/eqn.c                  src/main.c                 src/mygetopt.c          \
               src/reader.c               src/writer.c               src/init.c
               
HDRS         = src/cole.h                 src/cole_support.h         src/eqn.h               \
               src/eqn_support.h          src/mygetopt.h             src/rtf2latex2e.h       \
               src/rtf.h                  src/init.h

RTFPREP_SRCS = src/rtf-controls           src/rtfprep.c              src/standard-names      \
               src/tokenscan.c            src/tokenscan.h            src/rtf-ctrldef.h       \
               src/rtf-namedef.h          src/stdcharnames.h

RTFPREP_OBJS = src/rtfprep.o              src/tokenscan.o

PREFS        = pref/latex-encoding                pref/latex-encoding.mac            \
               pref/latex-encoding.cp1250         pref/latex-encoding.cp1252         \
               pref/latex-encoding.latin1         pref/latex-encoding.german         \
               pref/rtf-encoding.mac              pref/rtf-encoding.cp437            \
               pref/rtf-encoding.cp850            pref/rtf-encoding.cp1250           \
               pref/rtf-encoding.cp1252           pref/rtf-encoding.cp1254           \
               pref/rtf-encoding.next             pref/rtf-encoding.symbolfont       \
               pref/rtf-ctrl                      pref/r2l-head                      \
               pref/r2l-map                       pref/r2l-pref                      \
               pref/cp936raw.txt                  pref/cp932raw.txt                  \
               pref/rtf-encoding.cp1251           

DOCS         = doc/GPL_license      doc/ChangeLog     \
               doc/rtf2latexDoc.tex doc/rtf2latex2e.1 \
               doc/Makefile                           \
               doc/web/style.css    doc/web/docs.html \
               doc/web/index.html   doc/web/logo.png  \
               doc/web/arrow.gif
               
PDFS         = doc/rtf2latexDoc.pdf  

TEST         = test/Makefile              test/arch.rtf              test/arch-mac.rtf       \
               test/equation.rtf          test/fig-jpeg.rtf          test/multiline.rtf      \
               test/mapping.rtf           test/rtf-misc.rtf          test/rtf.rtf            \
               test/table.rtf             test/test.rtf              test/moreEqns.rtf       \
               test/twoEqn.rtf            test/science.rtf           test/russian-short.rtf  \
               test/enc-utf8x.rtf \
               test/RtfInterpreterTest_0.rtf  test/RtfInterpreterTest_4.rtf  test/RtfInterpreterTest_8.rtf \
               test/RtfInterpreterTest_1.rtf  test/RtfInterpreterTest_5.rtf  test/RtfInterpreterTest_9.rtf \
               test/RtfInterpreterTest_2.rtf  test/RtfInterpreterTest_6.rtf  test/RtfInterpreterTest_10.rtf \
               test/RtfInterpreterTest_3.rtf  test/RtfInterpreterTest_7.rtf  test/RtfInterpreterTest_11.rtf 


RTFD         = test/sample.rtfd/TXT.rtf      test/sample.rtfd/amiga.gif \
               test/sample.rtfd/build.tiff   test/sample.rtfd/button_smile.jpeg \
               test/sample.rtfd/paste.eps

EQNS         = test/testeqn01.eqn         test/testeqn02.eqn         test/testeqn03.eqn      \
               test/testeqn04.eqn         test/testeqn05.eqn         test/testeqn06.eqn      \
               test/testeqn07.eqn         test/testeqn08.eqn         test/testeqn09.eqn      \
               test/testeqn10.eqn
               
OBJS         = src/cole.o                 src/cole_decode.o          src/cole_support.o      \
               src/eqn.o                  src/main.o                 src/mygetopt.o          \
               src/reader.o               src/tokenscan.o            src/writer.o            \
               src/init.o

all : checkfiles rtf2latex2e

src/rtfprep: src/tokenscan.o src/rtfprep.o
	$(CC) $(PLATFORM) $(LDFLAGS) $(CFLAGS) $(RTFPREP_OBJS) -o src/rtfprep

rtf2latex2e: $(OBJS) $(HDRS)
	$(CC) $(PLATFORM) $(CFLAGS) $(OBJS) -o $(BINARY_NAME)
	cp -f $(BINARY_NAME) rtf2latex

src/cole.o: src/cole.c src/cole.h src/cole_support.h
	$(CC) $(PLATFORM) $(CFLAGS) -c src/cole.c -o src/cole.o

src/cole_decode.o: src/cole_decode.c src/cole.h src/cole_support.h
	$(CC) $(PLATFORM) $(CFLAGS) -c src/cole_decode.c -o src/cole_decode.o

src/cole_support.o: src/cole_support.c src/cole_support.h
	$(CC) $(PLATFORM) $(CFLAGS) -c src/cole_support.c -o src/cole_support.o

src/eqn.o: src/eqn.c src/rtf.h src/rtf-ctrldef.h src/rtf-namedef.h src/rtf2latex2e.h src/cole_support.h src/eqn.h src/eqn_support.h
	$(CC) $(PLATFORM) $(CFLAGS) -c src/eqn.c -o src/eqn.o

src/init.o: src/init.c
	$(CC) $(PLATFORM) -DPREFS_DIR=\"$(prefsdir)\" $(CFLAGS) -c src/init.c -o src/init.o

src/main.o: src/main.c
	$(CC) $(PLATFORM) -DPREFS_DIR=\"$(prefsdir)\" -DVERSION=\"$(VERSION)\" $(CFLAGS) -c src/main.c -o src/main.o

src/mygetopt.o: src/mygetopt.c src/mygetopt.h
	$(CC) $(PLATFORM) $(CFLAGS) -c src/mygetopt.c -o src/mygetopt.o

src/reader.o: src/reader.c src/tokenscan.h src/rtf.h src/rtf-ctrldef.h src/rtf-namedef.h src/rtf2latex2e.h src/stdcharnames.h
	$(CC) $(PLATFORM) $(CFLAGS) -c src/reader.c -o src/reader.o

src/tokenscan.o: src/tokenscan.c src/tokenscan.h
	$(CC) $(PLATFORM) $(CFLAGS) -c src/tokenscan.c -o src/tokenscan.o

src/writer.o: src/writer.c src/rtf.h src/rtf-ctrldef.h src/rtf-namedef.h src/tokenscan.h src/cole.h src/cole_support.h src/rtf2latex2e.h src/eqn.h
	$(CC) $(PLATFORM) $(CFLAGS) -c src/writer.c -o src/writer.o

pdf : doc/rtfReader.tex doc/rtf2latexDoc.tex
	cd doc && $(MAKE)

check:
test: rtf2latex2e
	cd test && $(MAKE) clean
	cd test && $(MAKE)

checkfiles: $(SRCS) $(RTFPREP_SRCS) $(HDRS) $(PREFS) $(TEST) $(DOCS) Makefile

depend: $(SRCS)
	$(CC) -MM $(SRCS) >makefile.depend
	@echo "***** Append makefile.depend to Makefile manually ******"

dist: checkfiles doc $(SRCS) $(RTFPREP_SRC) $(HDRS) $(README) $(PREFS) $(TEST) $(DOCS) Makefile convert2pdf
	make doc
	mkdir -p           rtf2latex2e-$(VERSION)
	mkdir -p           rtf2latex2e-$(VERSION)/pref
	mkdir -p           rtf2latex2e-$(VERSION)/doc
	mkdir -p           rtf2latex2e-$(VERSION)/test
	mkdir -p           rtf2latex2e-$(VERSION)/test/sample.rtfd
	mkdir -p           rtf2latex2e-$(VERSION)/src
	ln README          rtf2latex2e-$(VERSION)
	ln Makefile        rtf2latex2e-$(VERSION)
	ln convert2pdf     rtf2latex2e-$(VERSION)
	ln $(SRCS)         rtf2latex2e-$(VERSION)/src
	ln $(HDRS)         rtf2latex2e-$(VERSION)/src
	ln $(RTFPREP_SRCS) rtf2latex2e-$(VERSION)/src
	ln $(PREFS)        rtf2latex2e-$(VERSION)/pref
	ln $(DOCS)         rtf2latex2e-$(VERSION)/doc
	ln $(PDFS)         rtf2latex2e-$(VERSION)/doc
	ln $(TEST)         rtf2latex2e-$(VERSION)/test
	ln $(RTFD)         rtf2latex2e-$(VERSION)/test/sample.rtfd
	ln $(EQNS)         rtf2latex2e-$(VERSION)/test
	tar cvf - rtf2latex2e-$(VERSION) | gzip > rtf2latex2e-$(VERSION).tar.gz
#	zip -r rtf2latex2e-$(VERSION) rtf2latex2e-$(VERSION)
	rm -rf rtf2latex2e-$(VERSION)
	
install: rtf2latex2e
	mkdir -p                $(DESTDIR)$(bindir)
	mkdir -p                $(DESTDIR)$(datadir)/$(package-name)
	
	cp -f -p $(PREFS)             $(DESTDIR)$(datadir)/$(package-name)
	cp -f -p doc/rtf2latex2e.1    $(DESTDIR)$(mandir)/man1
	cp -f -p $(BINARY_NAME)       $(DESTDIR)$(bindir)
	cp -f -p convert2pdf          $(DESTDIR)$(bindir)
	
	@echo "******************************************************************"
	@echo "*** rtf2latex2e successfully installed as \"$(BINARY_NAME)\""
	@echo "*** in directory \"$(bindir)\""
	@echo "***"
	@echo "*** rtf2latex2e was compiled to search for its configuration files in"
	@echo "***           \"$(datarootdir)/$(package-name)\" "
	@echo "***"
	@echo "*** If the configuration files are moved then either"
	@echo "***   1) set the environment variable RTFPATH to this new location, or"
	@echo "***   2) use the command line option -P /path/to/prefs, or"
	@echo "***   3) edit the Makefile and recompile"
	@echo "******************************************************************"

install-prefs:
	cp -f -p $(PREFS)             $(DESTDIR)$(datadir)/$(package-name)

install-pdf: 
	mkdir -p $(pdfdir)
	cp -f -p doc/rtfReader.pdf    $(DESTDIR)$(pdfdir)
	cp -f -p doc/rtf2latexDoc.pdf $(DESTDIR)$(pdfdir)

clean: 
	rm -f $(OBJS) $(RTFPREP_OBJS) $(BINARY_NAME) rtf2latex
	cd test   && make clean
	cd doc    && make clean
	
realclean: checkfiles clean
	rm -f makefile.depend
	rm -f src/rtfprep
	cd test   && make realclean
	cd doc    && make realclean

parser: checkfiles clean
	rm -f src/rtfprep
	rm -f src/rtf-ctrldef.h  src/rtf-namedef.h  src/stdcharnames.h src/rtf-ctrl
	make src/rtfprep	
	cd src && ./rtfprep
	mv src/rtf-ctrl pref/rtf-ctrl

appleclean:
	sudo xattr -r -d com.apple.FinderInfo ./
	sudo xattr -r -d com.apple.TextEncoding ./
	sudo xattr -r -d com.apple.quarantine ./
	
splint: 
	splint -weak $(SRCS) $(HDRS)
	
.PHONY: all checkfiles clean depend dist doc install realclean test
