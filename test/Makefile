RTF2LATEX= ../rtf2latex2e
OPTIONS  = -T2 -P ../pref
LATEX= pdflatex -interaction=nonstopmode
LATEX_LOG = > latex.log
EQN_LOG =  &> eqn.log

TEX     =   arch-mac.tex arch.tex equation.tex  mapping.tex \
            rtf-misc.tex rtf.tex table.tex test.tex moreEqns.tex twoEqn.tex \
            science-latex/science.tex \
            multiline.tex enc-utf8x.tex \
            fig-jpeg-latex/fig-jpeg.tex \
            RtfInterpreterTest_0.tex   RtfInterpreterTest_2.tex   RtfInterpreterTest_6.tex\
            RtfInterpreterTest_1.tex   RtfInterpreterTest_3.tex   RtfInterpreterTest_7.tex\
            RtfInterpreterTest_10.tex  RtfInterpreterTest_4.tex   RtfInterpreterTest_8.tex\
            RtfInterpreterTest_11.tex  RtfInterpreterTest_5.tex   RtfInterpreterTest_9.tex \
            RtfInterpreterTest_12.tex  RtfInterpreterTest_13.tex  RtfInterpreterTest_14.tex \
            RtfInterpreterTest_15.tex  RtfInterpreterTest_16.tex  RtfInterpreterTest_17.tex \
            russian-short.tex

EQN     = testeqn01.eqn  testeqn02.eqn  testeqn03.eqn  testeqn04.eqn  \
          testeqn05.eqn  testeqn06.eqn  testeqn07.eqn  testeqn08.eqn  \
          testeqn09.eqn  testeqn10.eqn

TEX_EQN = testeqn01.tex  testeqn02.tex  testeqn03.tex  testeqn04.tex  \
          testeqn05.tex  testeqn06.tex  testeqn07.tex  testeqn08.tex  \
          testeqn09.tex  testeqn10.tex

PDF     = arch-mac.pdf arch.pdf equation.pdf mapping.pdf \
          rtf-misc.pdf rtf.pdf moreEqns.pdf twoEqn.pdf  multiline.pdf \
          fig-jpeg-latex/fig-jpeg.pdf \
          science-latex/science.pdf \
          testeqn01.pdf  testeqn02.pdf  testeqn03.pdf  testeqn04.pdf  \
          testeqn05.pdf  testeqn06.pdf  testeqn07.pdf  testeqn08.pdf  \
          testeqn09.pdf  testeqn10.pdf

TEX2    =   Example2.rtfd/TXT.tex \
            RtfParserTest_4.tex\
            RtfParserTest_5.tex\
            RtfParserTest_6.tex\
            RtfParserTest_7.tex\
            RtfParserTest_8.tex\
            RtfInterpreterTest_18.tex\
            RtfInterpreterTest_19.tex\
            RtfInterpreterTest_20.tex\
            RtfInterpreterTest_21.tex\
            RtfInterpreterTest_22.tex\
            RtfInterpreterTest_23.tex\
            RtfParserTest_0.tex        RtfParserTest_1.tex        RtfParserTest_2.tex\
            RtfParserTest_3.tex

FAIL     =  RtfInterpreterTest_fail_0.tex  RtfInterpreterTest_fail_4.tex  RtfParserTest_fail_3.tex \
            RtfInterpreterTest_fail_1.tex  RtfParserTest_fail_0.tex       RtfParserTest_fail_4.tex \
            RtfInterpreterTest_fail_2.tex  RtfParserTest_fail_1.tex       RtfParserTest_fail_5.tex \
            RtfInterpreterTest_fail_3.tex  RtfParserTest_fail_2.tex       RtfParserTest_fail_6.tex


all: $(TEX) 

eqn: $(TEX_EQN)

pdf: $(PDF)

Example2.rtfd/TXT.tex : 
	$(RTF2LATEX) $(OPTIONS) Example2.rtfd

Example2.rtfd/TXT.pdf : Example2.rtfd/TXT.tex
	cd Example2.rtfd && $(LATEX) TXT $(LATEX_LOG)

fig-jpeg-latex/fig-jpeg.tex : fig-jpeg.rtf
	$(RTF2LATEX) $(OPTIONS) -D fig-jpeg.rtf

fig-jpeg-latex/fig-jpeg.pdf : fig-jpeg-latex/fig-jpeg.tex
	cd fig-jpeg-latex && $(LATEX) fig-jpeg.tex $(LATEX_LOG)

science-latex/science.tex : science.rtf
	$(RTF2LATEX) $(OPTIONS) -D science.rtf

science-latex/science.pdf : science-latex/science.tex
	cd science-latex && $(LATEX) science $(LATEX_LOG)
	cd science-latex && $(LATEX) science $(LATEX_LOG)

%.tex:	%.rtf
	$(RTF2LATEX) $(OPTIONS) $<

%.tex:	%.eqn
	$(RTF2LATEX) $(OPTIONS) $< $(EQN_LOG)

%.pdf:  %.tex
	$(LATEX) $< $(LATEX_LOG)
	
clean:
	rm -rf *.tex *.pdf *.log *.aux
	rm -rf *.rtfd/TXT.tex *.rtfd/TXT.log *.rtfd/TXT.pdf *.rtfd/TXT.aux
	rm -rf fig-jpeg-latex science-latex
	
realclean: clean
	mkdir -p hide
	mv $(EQN) hide
	rm -rf *.dvi *.pict *.eps *.ps *.eqn *.jpg *.png *.wmf *.emf
	rm -rf *-latex
	mv hide/* .
	rmdir hide

.PHONY: clean all realclean
