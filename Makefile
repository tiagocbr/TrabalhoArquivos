all: funcoes_fornecidas.o reader.o regCabecalho.o indiceSimples.o programaTrab.o
	gcc funcoes_fornecidas.o reader.o regCabecalho.o indiceSimples.o programaTrab.o -Wall -std=c99 -o programaTrab

run: all
	./programaTrab > test.txt

programaTrab.o: programaTrab.c
	gcc -c programaTrab.c -o programaTrab.o

reader.o: reader.c reader.h
	gcc -c reader.c -o reader.o

indiceSimples.o: indiceSimples.c indiceSimples.h
	gcc -c indiceSimples.c -o indiceSimples.o


funcoes_fornecidas.o: funcoes_fornecidas.c funcoes_fornecidas.h
	gcc -c funcoes_fornecidas.c -o funcoes_fornecidas.o

regCabecalho.o: regCabecalho.c regCabecalho.h
	gcc -c regCabecalho.c -o regCabecalho.o


clean:
	rm *.o programaTrab binario.bin index.bin
