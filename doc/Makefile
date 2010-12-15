SRCS = Makefile Release-notes.txt GPL_license \
       rtf2LaTeX2eDoc.tex rtf2latex2eSWP.tex rtfReader.tex
HTML = rtf2LaTeX2eDoc.html rtf2latex2eSWP.html rtfReader.html
PDF  = rtf2LaTeX2eDoc.pdf rtf2latex2eSWP.pdf rtfReader.pdf

all: $(HTML) $(PDF)

%.html:	%.tex
	tth $<
	
%.pdf:	%.tex
	pdflatex $<

clean:
	$(RM) *.log *.aux *.out
	
realclean: clean
	$(RM) *.pdf *.html
