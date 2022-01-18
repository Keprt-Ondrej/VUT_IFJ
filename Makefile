PROGS = IFJ-Projekt-2021	#TODO je to spravne?
CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -g #-Werror #-O2 #TODO verze C??
LOGIN = xkeprt03
#CFLAGS += -fsanitize=address
#LDFLAGS += -fsanitize=address

OFILES = scanner.o parser.o symtable.o parser_precedence.o parser_recursive_descent.o code_generator.o
HFILES = scanner.h parser.h symtable.h parser_precedence.h parser_recursive_descent.h code_generator.h

all: $(PROGS)

$(PROGS): $(OFILES) $(HFILES) main.o
	$(CC) $(CFLAGS) -o  $(PROGS) $(OFILES) main.o

lex_test: scanner.o scanner.h lex_test_main.o
	$(CC) $(CFLAGS) -o lex_test $^ 

run: $(PROGS)
	./$(PROGS)

test: $(PROGS) #chmod +x tests.sh    -- aby se mohl script spustit jako program: ./tests.sh
	sh tests.sh	

doxygen:
	doxygen

pdf:
	$(MAKE) -C dokumentace/

pack:	
	zip $(LOGIN).zip doxyfile Makefile README.txt *.c *.h dokumentace.pdf rozdeleni

clean:
	$(MAKE) -C dokumentace/ clean
	rm -fr *.o $(PROGS) dokumentace.pdf
