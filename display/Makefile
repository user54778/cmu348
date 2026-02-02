CFLAGS= -Wall -pedantic -std=gnu99 -g

all: main

main: main.o babydriver.o gojo.o
	gcc $(CFLAGS) main.o babydriver.o gojo.o -o main

babydriver:
	gcc -c $(CFLAGS) babydriver.c -o babydriver.o

gojo: gojo.c
	gcc -c $(CFLAGS) gojo.c -o gojo.o

clean:
	rm -f *.o main
