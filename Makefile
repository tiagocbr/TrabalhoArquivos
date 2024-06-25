all: funcoes_fornecidas.o reader.o regCabecalho.o indiceSimples.o util.o programaTrab.o arvore_b.o
	gcc funcoes_fornecidas.o util.o reader.o regCabecalho.o indiceSimples.o programaTrab.o arvore_b.o -Wall -std=c99 -o programaTrab

run: all
	./programaTrab

arvore_b.o: arvore_b.c
	gcc -c arvore_b.c -o arvore_b.o

programaTrab.o: programaTrab.c
	gcc -c programaTrab.c -o programaTrab.o

util.o: util.c
	gcc -c util.c -o util.o

reader.o: reader.c reader.h
	gcc -c reader.c -o reader.o

indiceSimples.o: indiceSimples.c indiceSimples.h
	gcc -c indiceSimples.c -o indiceSimples.o


funcoes_fornecidas.o: funcoes_fornecidas.c funcoes_fornecidas.h
	gcc -c funcoes_fornecidas.c -o funcoes_fornecidas.o

regCabecalho.o: regCabecalho.c regCabecalho.h
	gcc -c regCabecalho.c -o regCabecalho.o

eraseBin:
	rm *.bin

debug7:
	cp arquivos/files/binario3.bin .
	cp arquivos/files/indice3.bin .

allValgrind: funcoes_fornecidas.o reader.o regCabecalho.o indiceSimples.o util.o programaTrab.o
	gcc funcoes_fornecidas.o util.o reader.o regCabecalho.o indiceSimples.o programaTrab.o -g -Wall -std=c99 -o programaTrab

runValgrind: allValgrind
	valgrind -s --tool=memcheck --leak-check=yes ./programaTrab

clean: 
	rm *.o programaTrab
