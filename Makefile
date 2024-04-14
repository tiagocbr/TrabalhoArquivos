all: reader.o regCabecalho.o programaTrab.o
	gcc reader.o programaTrab.o -Wall -std=c99 -o programaTrab

run:
	./programaTrab

programaTrab.o: programaTrab.c
	gcc -c programaTrab.c -o programaTrab.o

regCabecalho.o: regCabecalho.c regCabecalho.h
	gcc -c regCabecalho.c -o regCabecalho.o

reader.o: reader.c reader.h
	gcc -c reader.c -o reader.o

clean:
	rm *.o programaTrab
