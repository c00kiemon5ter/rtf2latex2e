SRCS = Makefile Release-notes.txt GPL_license \
       rtf2LaTeX2eDoc.tex rtf2latex2eSWP.tex rtfReader.tex
PDF  = rtf2LaTeX2eDoc.pdf rtf2latex2eSWP.pdf rtfReader.pdf

all: $(PDF)

%.pdf:	%.tex
	pdflatex $<

clean:
	$(RM) *.log *.aux *.out
	
realclean: clean
	$(RM) *.pdf *.html
