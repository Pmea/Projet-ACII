CC= gcc
CFLAGS= -Wall -Werror -g -lm

#Excercice 1
t-test: main-pop.o textuel-pop.o
	$(CC) -o $@ $^ $(CFLAGS) 

main-pop.o: main-pop.c 
	$(CC) -o $@ -c $< $(CFLAGS)		

textuel-pop.o: textuel-pop.c textuel-pop.h
		$(CC) -o $@ -c $< $(CFLAGS)	

#Excercice 2
c-test: main-pop.o clicable-pop.o
	$(CC) -o $@ $^ $(CFLAGS) 

main-pop.o: main-pop.c 
	$(CC) -o $@ -c $< $(CFLAGS)		

clicable-pop.o: clicable-pop.c clicable-pop.h
		$(CC) -o $@ -c $< $(CFLAGS)	

#Excercice 3
g-test: main-pop.o graphique-pop.o
	$(CC) -o $@ $^ $(CFLAGS) 

main-pop.o: main-pop.c 
	$(CC) -o $@ -c $< $(CFLAGS)		

graphique-pop.o: graphique-pop.c graphique-pop.h
		$(CC) -o $@ -c $< $(CFLAGS)	

clean:
	rm main-pop *.o 