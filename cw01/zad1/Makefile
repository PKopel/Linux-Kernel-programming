CC = gcc -Wall

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so

build:clean
	$(CC) main.c -o main.o

run:build
	./main.o