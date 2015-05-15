CC= gcc
CFLAGS= -Wall -g -lm -lX11

DIR= /tmp/3402198


all: main-pop 


#General

$(DIR):
		mkdir $(DIR);

$(DIR)/main-pop.o: main-pop.c $(DIR)
	$(CC) -o $@ -c $< $(CFLAGS)		

$(DIR)/liste_mime.o: liste_mime.c liste_mime.h  $(DIR)
	$(CC) -o $@ -c $< $(CFLAGS)		

$(DIR)/basique-pop.o: basique-pop.c basique-pop.h $(DIR)
	$(CC) -o $@ -c $< $(CFLAGS)	

$(DIR)/cliquable-pop.o: cliquable-pop.c cliquable-pop.h $(DIR)
	$(CC) -o $@ -c $< $(CFLAGS)	

$(DIR)/graphique-pop.o: graphique-pop.c graphique-pop.h $(DIR)
	$(CC) -o $@ -c $< $(CFLAGS)	

main-pop: $(DIR)/main-pop.o $(DIR)/basique-pop.o $(DIR)/cliquable-pop.o $(DIR)/graphique-pop.o $(DIR)/liste_mime.o
	$(CC) -o $@ $^ $(CFLAGS)

#Excercice 1
t-test: main-pop 
	./main-pop 127.0.0.1 110 -t

#Excercice 2
c-test: main-pop
	./main-pop 127.0.0.1 110 -c

#Excercice 3
g-test: main-pop
	./main-pop 127.0.0.1 110 -g


clean:
	rm -fr main-pop *.o $(DIR)