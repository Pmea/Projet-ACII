CC= gcc
CFLAGS= -Wall -Werror -g -lm

#General
main-pop.o: main-pop.c 
	$(CC) -o $@ -c $< $(CFLAGS)		

liste_mime.o: liste_mime.c liste_mime.h 
	$(CC) -o $@ -c $< $(CFLAGS)		

#Excercice 1
t-test: main-pop.o textuel-pop.o liste_mime.o
	$(CC) -o $@ $^ $(CFLAGS) 

textuel-pop.o: textuel-pop.c textuel-pop.h
		$(CC) -o $@ -c $< $(CFLAGS)	

#Excercice 2
c-test: main-pop.o clicable-pop.o liste_mime.o
	$(CC) -o $@ $^ $(CFLAGS) 	

clicable-pop.o: clicable-pop.c clicable-pop.h
		$(CC) -o $@ -c $< $(CFLAGS)	

#Excercice 3
g-test: main-pop.o graphique-pop.o liste_mime.o
	$(CC) -o $@ $^ $(CFLAGS) 	

graphique-pop.o: graphique-pop.c graphique-pop.h
		$(CC) -o $@ -c $< $(CFLAGS)	

clean:
	rm t-test c-test g-test *.o 