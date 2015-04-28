CC= gcc
CFLAGS= -Wall -Werror -g -lm


all: main-pop

#General
main-pop.o: main-pop.c 
	$(CC) -o $@ -c $< $(CFLAGS)		

liste_mime.o: liste_mime.c liste_mime.h 
	$(CC) -o $@ -c $< $(CFLAGS)		

basique-pop.o: basique-pop.c basique-pop.h
	$(CC) -o $@ -c $< $(CFLAGS)	

cliquable-pop.o: cliquable-pop.c cliquable-pop.h
	$(CC) -o $@ -c $< $(CFLAGS)	

graphique-pop.o: graphique-pop.c graphique-pop.h
	$(CC) -o $@ -c $< $(CFLAGS)	

main-pop: main-pop.o basique-pop.o cliquable-pop.o graphique-pop.o liste_mime.o
	$(CC) -o $@ $^ $(CFLAGS) 

#Excercice 1
t-test: main-pop 
	textuel


#Excercice 2
c-test: main-pop
	cliquable

#Excercice 3
g-test: main-pop
	graphique


clean:
	rm t-test c-test g-test *.o 