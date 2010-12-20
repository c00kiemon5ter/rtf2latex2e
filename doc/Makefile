SRCS = Makefile Release-notes.txt GPL_license \
       rtf2latex2eDoc.tex rtf2latex2eSWP.tex rtfReader.tex
PDF  = rtf2latex2eDoc.pdf rtf2latex2eSWP.pdf rtfReader.pdf

all: $(PDF)

%.pdf:	%.tex
	pdflatex $<

rtf2latex2eSWP.pdf : rtf2latex2eSWP.tex
	pdflatex rtf2latex2eSWP

rtfReader.pdf : rtfReader.tex
	pdflatex rtfReader

rtf2latex2eDoc.pdf : rtf2latex2eDoc.tex
	pdflatex rtf2latex2eDoc

clean:
	$(RM) *.log *.aux *.out
	
realclean: clean
	$(RM) *.pdf
