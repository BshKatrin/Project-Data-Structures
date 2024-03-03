CC=gcc
CFLAGS=-Wall
PROGRAMS=ChaineMain

all : $(PROGRAMS)

Chaine.o : Chaine.c
	$(CC) $(CFLAGS) -c -o $@ $^

ChaineMain: ChaineMain.c Chaine.o SVGLib/SVGwriter.o
	$(CC) $(CFLAGS) -o $@ $^

SVGLib/SVGwriter.o : SVGLib/SVGwriter.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean : 
	rm *.o SVGLib/*.o $(PROGRAMS) 
