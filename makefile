PROGS = IFJ-Projekt-2021	#TODO je to spravne?
CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -g -Werror #-O2 #TODO verze C??
LOGIN = xkeprt03
#CFLAGS += -fsanitize=address
#LDFLAGS += -fsanitize=address

OFILES = main.o symtable.o scanner.o
HFILES = main.h

all: $(PROGS)

$(PROGS): $(OFILES) $(HFILES)
	$(CC) $(CFLAGS) -o  $(PROGS) $(OFILES)	

doxygen:
	doxygen

pack:	#zkuste to psat zvrchu dolu, jak to mate napr ve VS code at v tom neni bordel :D
	zip $(LOGIN).zip doxyfile main.c main.h makefile README.txt scanner.c scanner.h symtable.c symtable.h

clean:
	rm -fr *.o html/ $(PROGS)