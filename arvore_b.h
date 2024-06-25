#ifndef ARVORE_B_H
    #define ARVORE_B_H
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include <string.h>
    
    typedef long long ll;
    typedef struct arvore_b ARVORE_B;

    ARVORE_B* arvore_criar(char *indices);
    ARVORE_B *arvore_carregar_cabecalho(char *nomeArquivo);
    void arvore_destruir(ARVORE_B** arvore);

    //retorna o byteOffSet do elemento encontrado, ou -1 caso nao tenha sido encontrado
    ll arvore_buscar(ARVORE_B* arvore, int chave);


    bool arvore_inserir(ARVORE_B* arvore, int chave, ll offset);

    void imprime_arvore(ARVORE_B* arvore,FILE* indices);
    void imprimeCabecalho(ARVORE_B *arv) ;

#endif