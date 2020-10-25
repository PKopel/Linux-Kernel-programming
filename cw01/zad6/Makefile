CC = gcc -Wall

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so

build:clean
	$(CC) main.c ../common.c -o main.o -lpthread

run:build
	./main.o