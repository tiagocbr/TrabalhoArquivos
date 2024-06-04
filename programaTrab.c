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
    bool res=true;             // Armzaena os resultados das funcionalidades booleanas
    
    scanf("%d", &opCode);
    switch(opCode) {
        case 1:
            scanf(" %s", arqEntrada);
            scanf(" %s", arqSaida);
            res = reader_create_table(arqEntrada, arqSaida);
            break;

        case 2:
            scanf(" %s", arqEntrada);
            res = reader_select_from(arqEntrada);
            break;

        case 3:
            scanf(" %s", arqEntrada);
            scanf("%d", &n);
            res = reader_select_where(arqEntrada, n);
            break;

        case 4:
            scanf(" %s", arqEntrada);
            scanf(" %s", arqSaida);
            res = reader_create_index(arqEntrada, arqSaida);
            if(res)binarioNaTela(arqSaida);
            break;
        case 5:
            scanf(" %s", arqEntrada);
            scanf(" %s",arqSaida);
            scanf(" %d",&n);
            res = reader_delete_where(arqEntrada,arqSaida,n);
            break;
        case 6:
            scanf(" %s", arqEntrada);
            scanf(" %s",arqSaida);
            scanf(" %d",&n);
            res = reader_insert_into(arqEntrada,arqSaida,n);
            break;
    }

    if(!res)
        printf("Falha no processamento do arquivo.\n");

    return 0;
}