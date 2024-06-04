#ifndef CABECALHO_H
    #define CABECALHO_H
    #include <stdio.h>
    #include <stdbool.h>
    #include <stdlib.h>

    // Implementa um tipo para armazenar as informções do registro
    // de cabeçalho de um arquivo
    typedef struct cabecalho CABECALHO;

    // Cria um novo cabeçalho
    CABECALHO *cabecalho_criar(void); //com valores padrao
    CABECALHO* cabecalho_from_arquivo(FILE* arquivo); //com valores de acordo com um arquivo
    
    // Conjunto de funções que retornam os valores dos campos de um cabeçalho
    char cabecalho_get_status(CABECALHO *c);
    long long cabecalho_get_topo(CABECALHO *c);
    long long cabecalho_get_proxOffset(CABECALHO *c);
    int cabecalho_get_nroRegArq(CABECALHO *c);
    int cabecalho_get_nroRegRem(CABECALHO *c);

    // Conjunto de funções que alteram os valores dos campos de um cabeçalho
    bool cabecalho_set_status(CABECALHO *c);
    bool cabecalho_set_topo(CABECALHO *c, long long novoTopo);
    bool cabecalho_set_proxOffset(CABECALHO *c, long long novoOffset);
    bool cabecalho_set_nroRegArq(CABECALHO *c, int novoNumero);
    bool cabecalho_set_nroRegRem(CABECALHO *c, int novoNumero);

    // Apaga o cabeçalho, desalocando a memória usada.
    bool cabecalho_apagar(CABECALHO **c);

#endif