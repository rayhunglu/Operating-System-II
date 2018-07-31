# filename = project1writeup

default: concurrency1

concurrency1.o: concurrency1.c
	gcc -o mt19937ar.o -c mt19937ar.c

concurrency1: concurrency1.o
	gcc -g concurrency1.c mt19937ar.o -o concurrency1 -lpthread
	rm -f mt19937ar.o
