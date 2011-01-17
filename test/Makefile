RTF2LATEX= ../rtf2latex2e
OPTIONS  = -P ../pref
LATEX= pdflatex -interaction=nonstopmode
LATEX_LOG = > latex.log
EQN_LOG =  &> eqn.log

TEX     = arch-mac.tex arch.tex equation.tex  mapping.tex \
          rtf-misc.tex rtf.tex table.tex test.tex moreEqns.tex twoEqn.tex \
          science-latex/science.tex \
          multiline.tex enc-utf8x.tex \
          fig-jpeg-latex/fig-jpeg.tex \
          Example2.rtfd/TXT.tex

EQN     = testeqn01.eqn  testeqn02.eqn  testeqn03.eqn  testeqn04.eqn  \
          testeqn05.eqn  testeqn06.eqn  testeqn07.eqn  testeqn08.eqn  \
          testeqn09.eqn  testeqn10.eqn

TEX_EQN = testeqn01.tex  testeqn02.tex  testeqn03.tex  testeqn04.tex  \
          testeqn05.tex  testeqn06.tex  testeqn07.tex  testeqn08.tex  \
          testeqn09.tex  testeqn10.tex

PDF     = arch-mac.pdf arch.pdf equation.pdf mapping.pdf \
          rtf-misc.pdf rtf.pdf moreEqns.pdf twoEqn.pdf  multiline.pdf \
          fig-jpeg-latex/fig-jpeg.pdf \
          Example2.rtfd/TXT.pdf\
          science-latex/science.pdf \
          testeqn01.pdf  testeqn02.pdf  testeqn03.pdf  testeqn04.pdf  \
          testeqn05.pdf  testeqn06.pdf  testeqn07.pdf  testeqn08.pdf  \
          testeqn09.pdf  testeqn10.pdf

all: $(TEX) $(PDF)

eqn: $(TEX_EQN)

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
	rm -rf *.dvi *.pict *.eps *.ps *.eqn *.jpg *.png
	rm -rf *-latex
	mv hide/* .
	rmdir hide

.PHONY: clean all realclean
