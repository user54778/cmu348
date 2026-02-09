CFLAGS= -Wall -pedantic -std=gnu99 -g

all: main

main: main.o babydriver.o gojo.o kuromi.o fonts.o
	gcc $(CFLAGS) main.o babydriver.o gojo.o kuromi.o fonts.o -o main

babydriver:
	gcc -c $(CFLAGS) babydriver.c -o babydriver.o

gojo: gojo.c
	gcc -c $(CFLAGS) gojo.c -o gojo.o

kuromi: kuromi.c
	gcc -c $(CFLAGS) kuromi.c -o kuromi.o

fonts: fonts.c
	gcc -c $(CFLAGS) fonts.c -o fonts.o

clean:
	rm -f *.o main
