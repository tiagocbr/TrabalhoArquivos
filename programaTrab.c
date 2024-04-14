#include <stdio.h>
#include "reader.h"

int main() {
    int opCode;           // armazena a funcionalidade escolhida pelo usuário
    char arqEntrada[30];  // Armazena o nome do arquivo de entrada
    char arqSaida[30];    // Armazena o nome do arquivo de saída
    int n = 0;            // Usada para informar a qntd de buscas de algumas funcionalidades
    bool res1;            // Armzaena os resultados das funcionalidades booleanas
    int res2;             // Armazena os resultados das funcionalidades inteiras
    
    scanf("%d", &opCode);
    switch(opCode) {
        case 1:
            scanf(" %s", arqEntrada);
            scanf(" %s", arqSaida);
            res1 = reader_create_table(arqEntrada, arqSaida);

            if(!res1)
                printf("Falha no processamento do arquivo\n");
            break;
        
        case 2:
            scanf(" %s", arqEntrada);
            res2 = reader_select_from(arqEntrada);

            if(res2 == 0)
                printf("Registro inexistente.\n");
            else if(res2 == -1)
                printf("Falha no processamento do arquivo\n");
            break;

        /*case 3:
            scanf(" %s", arqEntrada);
            scanf("%d", &n);
            res2 = reader_select_where(arqEntrada, n);
            if(res2 == 0)
                printf("Registro inexistente.\n");
            else if(res2 == -1)
                printf("Falha no processamento do arquivo\n");
            break;*/
    }

    return 0;
}