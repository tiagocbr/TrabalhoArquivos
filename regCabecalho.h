#ifndef CABECALHO_H
    #define CABECALHO_H
    #include <stdio.h>
    #include <stdbool.h>
    #include <stdlib.h>

    typedef struct cabecalho CABECALHO;

    CABECALHO *cabecalho_criar(void);

    char cabecalho_get_status(CABECALHO *c);
    long long cabecalho_get_topo(CABECALHO *c);
    long long cabecalho_get_proxOffset(CABECALHO *c);
    int abecalho_get_nroRegArq(CABECALHO *c);
    int cabecalho_get_nroRegRem(CABECALHO *c);

    bool cabecalho_set_status(CABECALHO *c);
    //bool cabecalho_set_topo(CABECALHO *c);
    bool cabecalho_set_proxOffset(CABECALHO *c, int novoOffset);
    bool cabecalho_set_nroRegArq(CABECALHO *c, int novoNumero);
    bool cabecalho_set_nroRegRem(CABECALHO *c, int novoNumero);

    bool cabecalho_apagar(CABECALHO **c);

#endif