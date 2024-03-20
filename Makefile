CC=gcc
CFLAGS=-Wall
PROGRAMS=ChaineMain ReconstitueReseau

all : $(PROGRAMS)

SVGLib/SVGwriter.o : SVGLib/SVGwriter.c
	$(CC) $(CFLAGS) -c -o $@ $^

Chaine.o : Chaine.c
	$(CC) $(CFLAGS) -c -o $@ $^

ChaineMain : ChaineMain.c Chaine.o SVGLib/SVGwriter.o
	$(CC) $(CFLAGS) -o $@ $^

Reseau.o : Reseau.c
	$(CC) $(CFLAGS) -c -o $@ $^

Hachage.o : Hachage.c
	$(CC) $(CFLAGS) -c -o $@ $^
	
ArbreQuat.o : ArbreQuat.c
	$(CC) $(CFLAGS) -c -o $@ $^

ReconstitueReseau : ReconstitueReseau.c Chaine.o Reseau.o SVGLib/SVGwriter.o Hachage.o ArbreQuat.o
	$(CC) $(CFLAGS) -o $@ $^

clean : 
	rm *.o SVGLib/*.o $(PROGRAMS) *.res *.html
