CC=gcc
CFLAGS=-Wall
PROGRAMS=ChaineMain

all : $(PROGRAMS)

Chaine.o : Chaine.c
	$(CC) $(CFLAGS) -c -o $@ $^

ChaineMain.o: ChaineMain.c
	$(CC) $(CFLAGS) -c -o $@ $^

ChaineMain: ChaineMain.o Chaine.o
	$(CC) $(CFLAGS) -o $@ $^

clean : 
	rm *.o $(PROGRAMS)
