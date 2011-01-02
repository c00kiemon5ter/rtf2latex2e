RTF2LATEX= ../rtf2latex2e
OPTIONS  = -P ../pref

TEX     = arch-mac.tex arch.tex Equation.tex fig-jpeg.tex mapping.tex \
          rtf-misc.tex rtf.tex table.tex test.tex moreEqns.tex twoEqn.tex \
          multiline.tex

EQN     = testeqn01.eqn  testeqn02.eqn  testeqn03.eqn  testeqn04.eqn  \
          testeqn05.eqn  testeqn06.eqn  testeqn07.eqn  testeqn08.eqn  \
          testeqn09.eqn  testeqn10.eqn

TEX_EQN = testeqn01.tex  testeqn02.tex  testeqn03.tex  testeqn04.tex  \
          testeqn05.tex  testeqn06.tex  testeqn07.tex  testeqn08.tex  \
          testeqn09.tex  testeqn10.tex

all: $(TEX)

eqn: $(TEX_EQN)

%.tex:	%.rtf
	$(RTF2LATEX) $(OPTIONS) $<

%.tex:	%.eqn
	$(RTF2LATEX) $(OPTIONS) $<
	
clean:
	rm -rf *.tex *.eps
	
realclean: clean
	mkdir -p hide
	mv $(TEX_EQN) hide
	rm -rf *.log *.aux *.pdf *.dvi *.pict *.eps *.ps *.eqn *.jpg *.png
	mv hide/* .
	rmdir hide

.PHONY: clean all realclean
