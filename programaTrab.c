#include <stdio.h>
#include "reader.h"

int main() {
    int opCode;           // armazena a funcionalidade escolhida pelo usuário
    char arqEntrada[30];  // Armazena o nome do arquivo de entrada
    char arqSaida[30];    // Armazena o nome do arquivo de saída
    int n = 0;                // Usada para informar a qntd de buscas de algumas funcionalidades
    bool res;             // Armzaena os resultados das funcionalidades
    
    scanf("%d", &opCode);
    switch(opCode) {
        case 1:
            scanf(" %s", arqEntrada);
            scanf(" %s", arqSaida);
            res = reader_create_table(arqEntrada, arqSaida);

            if(!res)
                printf("Falha no processamento do arquivo");
    }
    reader_create_table("jogador.csv", "binario.bin");
}