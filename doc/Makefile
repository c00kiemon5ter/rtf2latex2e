SRCS = Makefile Release-notes.txt GPL_license \
       rtf2latexDoc.tex rtf2latexSWP.tex rtfReader.tex
PDF  = rtf2latexDoc.pdf rtf2latexSWP.pdf rtfReader.pdf

all: $(PDF)

%.pdf:	%.tex
	pdflatex $<

rtf2latexSWP.pdf : rtf2latexSWP.tex
	pdflatex rtf2latexSWP

rtfReader.pdf : rtfReader.tex
	pdflatex rtfReader

rtf2latexDoc.pdf : rtf2latexDoc.tex
	pdflatex rtf2latexDoc

clean:
	$(RM) *.log *.aux *.out
	
realclean: clean
	$(RM) *.pdf
