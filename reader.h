#ifndef READER_H
    #define READER_H
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>

    typedef struct cabecalho CABECALHO;
    typedef struct registro REGIasd;
    asdasdas;
    // Cria o aqruivo com os registros do csv. recebe os nomes do arquivo de
    // leitura e do de sáida, e retorna um bool para o sucesso da operação
    bool reader_create_table(char *csv, char *binario);

    // Imprime todos o registros do arquivo binário.
    bool reader_select_from(char *binario);

    // Imprime todos os registros do arquivo segundo uma quantidade qntd
    // de buscas com condições próprias
    bool reader_select_where(char * binario, int qntd);


#endif