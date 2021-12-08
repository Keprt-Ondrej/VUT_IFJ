PROJEKT = dokumentace

all:
	#latex $(PROJEKT).tex
	#dvips -t a4 $(PROJEKT).dvi
	#ps2pdf $(PROJEKT).ps
	
	latex $(PROJEKT).tex
	bibtex $(PROJEKT)
	latex $(PROJEKT).tex
	latex $(PROJEKT).tex
	dvips -t a4 $(PROJEKT).dvi
	ps2pdf $(PROJEKT).ps
	mv dokumentace.pdf ../

.PHONY: 
clean:
	rm -rf *.dvi *.ps dokumentace.pdf *.aux *.log *fls *out *xmk *gz *toc html/ *.bbl *.blg