RTF2LATEX= ../rtf2latex2e
OPTIONS  = -P ../pref

TEX= arch-mac.tex arch.tex Equation.tex fig-jpeg.tex mapping.tex \
     rtf-misc.tex rtf.tex table.tex test.tex

HARDTEX = multiline.tex 

all: $(TEX)

%.tex:	%.rtf
	$(RTF2LATEX) $(OPTIONS) $<
	
clean:
	rm -rf *.tex *.eps
	
realclean: clean
	rm -rf *.log *.aux *.pdf *.dvi *.pict *.eps *.ps *.eqn *.jpg *.png

.PHONY: clean all realclean
