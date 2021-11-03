PROGS = IFJ-Projekt-2021	#TODO je to spravne?
CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -g -Werror #-O2 #TODO verze C??
LOGIN = xkeprt03
#CFLAGS += -fsanitize=address
#LDFLAGS += -fsanitize=address

OFILES = main.o symtable.o scanner.o parser.o
HFILES = main.h

all: $(PROGS)

$(PROGS): $(OFILES) $(HFILES)
	$(CC) $(CFLAGS) -o  $(PROGS) $(OFILES)

run: $(PROGS)
	./$(PROGS)

test: $(PROGS) #chmod +x tests.sh    -- aby se mohl script spustit jako program: ./tests.sh
	sh tests.sh	

doxygen:
	doxygen

pdf:
	$(MAKE) -C dokumentace/

pack: pdf	#zkuste to psat zvrchu dolu, jak to mate napr ve VS code at v tom neni bordel :D
	zip $(LOGIN).zip doxyfile Makefile README.txt *.c *.h dokumentace.pdf rozdeleni

clean:
	$(MAKE) -C dokumentace/ clean
	rm -fr *.o $(PROGS) dokumentace.pdf
