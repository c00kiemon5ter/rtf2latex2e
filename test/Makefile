RTF2LATEX= ../rtf2latex2e
OPTIONS  = -P ../pref

TEX= arch-mac.tex arch.tex Equation.tex fig-jpeg.tex mapping.tex \
     rtf-misc.tex rtf.tex table.tex test.tex moreEqns.tex twoEqn.tex

HARDTEX = multiline.tex 

all: $(TEX)

%.tex:	%.rtf
	$(RTF2LATEX) $(OPTIONS) $<
	
clean:
	rm -rf *.tex *.eps
	
realclean: clean
	mv testeqn01.eqn testeqn01.eqnx
	mv testeqn02.eqn testeqn02.eqnx
	mv testeqn03.eqn testeqn03.eqnx
	mv testeqn04.eqn testeqn04.eqnx
	mv testeqn05.eqn testeqn05.eqnx
	mv testeqn06.eqn testeqn06.eqnx
	rm -rf *.log *.aux *.pdf *.dvi *.pict *.eps *.ps *.eqn *.jpg *.png
	mv testeqn01.eqnx testeqn01.eqn
	mv testeqn02.eqnx testeqn02.eqn
	mv testeqn03.eqnx testeqn03.eqn
	mv testeqn04.eqnx testeqn04.eqn
	mv testeqn05.eqnx testeqn05.eqn
	mv testeqn06.eqnx testeqn06.eqn

.PHONY: clean all realclean
