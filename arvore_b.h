#ifndef ARVORE_B_H
    #define ARVORE_B_H
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include <string.h>
    
    typedef long long ll
    typedef struct arvore_b ARVORE_B;

    ARVORE_B* arvore_criar();
    void arvore_destruir(ARVORE_B** arvore);

    //retorna o byteOffSet do elemento encontrado, ou -1 caso nao tenha sido encontrado
    ll arvore_buscar(ARVORE_B* arvore, int chave);

    //retorna o byteOffset do elemento inserido
    bool arvore_inserir(ARVORE_B* arvore, int chave, ll offset);

#endif