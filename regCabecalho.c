#include "regCabecalho.h"

#define ERRO -3200

struct cabecalho{
    char status;
    long long topo;
    long long proxByteOffset;
    int nroRegArq;
    int nroRegRem;
};

CABECALHO *cabecalho_criar(void) {
    CABECALHO *c;

    // Criando e inicializando os valores do registro de cabeçalho na memória principal
    c = (CABECALHO *) malloc(sizeof(CABECALHO));
    if(c != NULL) {
        c->status = '0';
        c->topo = -1;
        c->proxByteOffset = 25;
        c->nroRegArq = 0;
        c->nroRegRem = 0;
    }

    return c;
}

// FUNÇÕES QUE RETORNAM O VALOR DE UM CAMPO DO REGISTRO ("GET")
// Retorna x caso c == NULL;
char cabecalho_get_status(CABECALHO *c) {
    return ((c != NULL) ? c->status : 'x');
}

long long cabecalho_get_topo(CABECALHO *c) {
    return ((c != NULL) ? c->topo : ERRO);
}

long long cabecalho_get_proxOffset(CABECALHO *c) {
    return ((c != NULL) ? c->proxByteOffset : ERRO);
}
int cabecalho_get_nroRegArq(CABECALHO *c) {
    return ((c != NULL) ? c->nroRegArq : ERRO);
}

int cabecalho_get_nroRegRem(CABECALHO *c) {
    return ((c != NULL) ? c->nroRegRem : ERRO);
}

// FUNÇÕES QUE ALTERAM O VALOR DE UM CAMPO DO REGISTRO ("SET")
// set_status não precisa de outro argumento, pois o status é sempre 0 ou 1
bool cabecalho_set_status(CABECALHO *c) {
    if(c != NULL) {
        if(c->status == '1')
            c->status = '0';
        else
            c->status = '1';

        return true;
    }
    return false;
}

bool cabecalho_set_topo(CABECALHO *c, int novoTopo) {
    if(c != NULL) {
        c->topo = novoTopo;
        return true;
    }
    return false;
}

bool cabecalho_set_proxOffset(CABECALHO *c, int novoOffset) {
    if(c != NULL) {
        c->proxByteOffset = novoOffset;
        return true;
    }
    return false;
}

bool cabecalho_set_nroRegArq(CABECALHO *c, int novoNumero) {
    if(c != NULL) {
        c->nroRegArq = novoNumero;
        return true;
    }
    return false;
}

bool cabecalho_set_nroRegRem(CABECALHO *c, int novoNumero) {
    if(c != NULL) {
        c->nroRegRem = novoNumero;
        return true;
    }
    return false;
}

bool cabecalho_apagar(CABECALHO **c) {
    if(*c != NULL) {
        free(*c);
        *c = NULL;

        return true;
    }
    return false;
}
