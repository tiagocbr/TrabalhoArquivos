#ifndef UTIL_H
    #define UTIL_H
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include <strings.h>
    #include "reader.h"
    #include "regCabecalho.h"
    #include "indiceSimples.h"

    // Arquivo com funções auxiliares e compartilhadas entre os outros .c


    // FUNÇÕES PARA MANIPULAR ARQUIVOS QUAISQUER
    // Função que verifica se um arquivo está consistente pelo seu status. Como ela 
    // checa se o ponteiro é nulo, as funções que a usam logo depois de fopen
    // Não precisam dessa verificação.
    bool consistente(FILE *ptrArquivo);

    // Função que percorre um csv para descobrir o seu número total de registros
    int get_numero_registros(FILE* arquivo);

    // Função que recebe um ponteiro para um arquivo aberto e muda seu status
    bool set_status_arquivo(FILE *arquivo, char status);


    // FUNÇÕES PARA MANIPULAR STRINGS
    // Função que pega uma string lida no csv e a converte em inteiro
    // (Usada para armazenar os campos id e idade)
    int strToInt(char *str, int tam);

    // Função que retorna o tamanho de uma string
    int get_tamanho_string(char *string);


    // FUNÇÕES DE ORDENAÇÃO
    // QuickSort para um vetor de REGISTROI
    void quickSort(REGISTROI *vet, int ini, int fim);


    // FUNÇÕES PARA DEBUG

    // Imprime um vetor de registrosi, seu número de registros e seu espaço alocado
    void imprimeVetRegistroi(VETREGISTROI *vet);

    // Imprime os elementos de um arquivo de índice simples no formado id: byteOffest
    void imprimeIndice(char *indice);
#endif