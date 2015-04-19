CC= gcc
CFLAGS= -Wall -Werror -g -lm

all: main-pop 
#t-test c-test g-test

#General 

main-pop: main-pop.o textuel-pop.o clicable-pop.o graphique-pop.o
	$(CC) -o $@ $^ $(CFLAGS) 

main-pop.o: main-pop.c 
	$(CC) -o $@ -c $< $(CFLAGS)		

textuel-pop.o: textuel-pop.c textuel-pop.h
		$(CC) -o $@ -c $< $(CFLAGS)	


#Excercice 1
#t-test: 

#Excercice 2
#c-test: 

#Excercice 3
#g-test:

clean:
	rm main-pop *.o *