/*
INTEGRANTES DO GRUPO:
Gabriel Antunes Afonso de Araujo - 14571077
Tiago Chaves Bezerra Rocha       - 14609637
*/

#include <stdio.h>
#include "reader.h"
#include "indiceSimples.h"

int main() {
    int opCode;           // armazena a funcionalidade escolhida pelo usuário
    char arqEntrada[30];  // Armazena o nome do arquivo de entrada
    char arqSaida[30];    // Armazena o nome do arquivo de saída
    int n = 0;            // Usada para informar a qntd de buscas de algumas funcionalidades
    bool res;             // Armzaena os resultados das funcionalidades booleanas
    
    scanf("%d", &opCode);
    switch(opCode) {
        case 1:
            scanf(" %s", arqEntrada);
            scanf(" %s", arqSaida);
            res = reader_create_table(arqEntrada, arqSaida);
        
        case 2:
            scanf(" %s", arqEntrada);
            res = reader_select_from(arqEntrada);

        case 3:
            scanf(" %s", arqEntrada);
            scanf("%d", &n);
            res = reader_select_where(arqEntrada, n);

        case 4:
            scanf(" %s", arqEntrada);
            scanf(" %s", arqSaida);
            res = reader_create_index(arqEntrada, arqSaida);
    }

    if(!res)
        printf("Falha no processamento do arquivo.\n");

    return 0;
}