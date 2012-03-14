SRCS = Makefile Release-notes.txt GPL_license \
       rtf2latexDoc.tex rtf2latex2e.1
WEB  = header.html footer.html web/index.html web/logo.png \
       web/arrow.gif web/style.css web/docs.html

all: rtf2latexDoc.pdf web/usage.html web/manpage.html

rtf2latexDoc.pdf : rtf2latexDoc.tex
	pdflatex rtf2latexDoc
	pdflatex rtf2latexDoc

web/manpage.html: header.html footer.html rtf2latex2e.1
	man ./rtf2latex2e.1 | man2html --pgsize=100000 --bare > mid.html
	# remove first four spaces from each lines
	perl -pi.bak -e 's/^ {2}//' mid.html
	cat header.html mid.html footer.html > man.html
	sed "s/HHTITLEHH/rtf2latex2e: man page/" man.html > web/manpage.html
	rm mid.html mid.html.bak man.html

web/usage.html: header.html footer.html rtf2latexDoc.tex
	pdflatex rtf2latexDoc
	pdflatex rtf2latexDoc
	hevea rtf2latexDoc.tex
	tail -n +50 rtf2latexDoc.html > temp1.html
	sed -n -e :a -e '1,7!{P;N;D;};N;ba' temp1.html > temp2.html
	cat header.html temp2.html footer.html > temp3.html
	sed "s/HHTITLEHH/rtf2latex2e: User's Manual/" temp3.html > web/usage.html
	rm -f temp1.html temp2.html temp3.html
	rm -f rtf2latexDoc.aux rtf2latexDoc.haux rtf2latexDoc.htoc
	rm -f rtf2latexDoc.log rtf2latexDoc.out rtf2latexDoc.html

install: web/usage.html web/manpage.html rtf2latexDoc.pdf
	cp rtf2latexDoc.pdf web
	rsync -ave ssh web/ web.sourceforge.net:/home/project-web/rtf2latex2e/htdocs
	
clean:
	$(RM) *.log *.aux *.out
	
realclean: clean
	$(RM) *.pdf
