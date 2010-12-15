RTF2LATEX= ../rtf2latex
OPTIONS  = -P ../pref

TEX= arch-mac.tex arch.tex equation.tex fig-jpeg.tex mapping.tex \
     multiline.tex rtf-misc.tex rtf.tex table.tex test.tex

all: $(TEX)

%.tex:	%.rtf
	$(RTF2LATEX) $(OPTIONS) $<
	
clean:
	rm -rf *.tex *.eps
	
realclean: clean
	rm -rf *.log *.aux *.pdf *.dvi *.pict *.eps *.ps

.PHONY: clean all realclean
