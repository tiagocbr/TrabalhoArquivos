all: reader.o programaTrab.o
	gcc reader.o programaTrab.o -Wall -std=c99 -o test.exe

programaTrab.o:
	gcc -c programaTrab.c -o programaTrab.o

reader.o:
	gcc -c reader.c -o reader.o

clean:
	rm reader.o main.o test.exe