all: reader.o programaTrab.o
	gcc reader.o programaTrab.o -Wall -std=c99 -o programaTrab

programaTrab.o: programaTrab.c
	gcc -c programaTrab.c -o programaTrab.o

reader.o: reader.c
	gcc -c reader.c -o reader.o

clean:
	rm *.o programaTrab

run:
	./programaTrab