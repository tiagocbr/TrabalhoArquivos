#ifndef READER_H
    #define READER_H
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include <string.h>

    // Implementa um tipo para o registro de dados, sendo usada tanto na
    // Manipulação do csv quanto do binário e suas funcionalidades.
    typedef struct registro REGISTRO;

    // Funcionalidade 1
    // Cria o aqruivo com os registros do csv. recebe os nomes do arquivo de
    // leitura e do de sáida, e retorna um bool para o sucesso da operação
    bool reader_create_table(char *csv, char *binario);

    // Funcionalidade 2
    // Imprime todos o registros do arquivo binário.
    bool reader_select_from(char *binario);

    // Funcionalidade 3
    // Imprime todos os registros do arquivo segundo uma quantidade qntd
    // de buscas com condições próprias
    bool reader_select_where(char * binario, int qntd);

    // Funcionalidade 4
    // Cria o arquivo de índice simples de um binário pré-existente
    bool reader_create_index(char *binario, char *indice);

    // Funcionalidade 5
    // Deleta um arquivo logicamente no arquivo principal e fisicamente no arquivo de índices
    // Esta funcionalidade será repetida n vezes
    bool reader_delete_where(char *binario ,char *indices,int n);

    // Funcionalidade 6
    // Insere novos registros no arquivo principal e no arquivo de índices
    // A inserção usa a estratégia best fit, buscando reaproveitar registros lógicamente
    // removidos no arquivo principal
    bool reader_insert_into(char* binario,char* indice,int n);

    //funcionalidade 7
    bool create_index_arvore_B(char* binario,char* indice);


    // Funcionalidade 8
    // Busca elementos do arquivo principal a partir do id e um arquivo de índices
    // criado com uma árvore b
    bool reader_select_from_id(char *binario, char *indice, int n);

    // Fucionalidade 9
    // Busca elementos a partir do id ou de outros parametros
    // Se for busca por id usa a Btree
    bool select_where_BTree(char* binario,char* indice, int n);


    // Funcionalidade 10
    // Insere um novo elemento no arquivo principal e no arquivo de índices com 
    // árvore b
    bool reader_insert_into_bTree(char *binario, char *indice, int n);



#endif