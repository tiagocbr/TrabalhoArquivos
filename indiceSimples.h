#ifndef INDICE_H
    #define INDICE_H
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>

    // Implementa um tipo de dado para armazenar os registros do arquivo de índices
    // Na memória pricipal
    struct registroi {
        int id;
        long long byteOffset;
    };
    typedef struct registroi REGISTROI;

    // Função auxiliar para criar na memória principal um registro do arquivo de ínidce
    // Apartir de um registro de dados lido
    REGISTROI indice_criar_registro(int id, long long offset);


    // FUNÇÕES PARA MANIPULAR O ÍNDICE SIMPLES NA MEMÓRIA

    // Realiza uma busca binária a partir de um id fornecido, retornando
    // O valor do índice no vetor de registros
    int indice_buscar(REGISTROI *registros, int idBuscado, int tamanho);

    // Insere ordenadamente um novo registro no vetor, sendo usada com a funcionalidade 6
    bool indice_inserir(REGISTROI *registros, REGISTROI novo, int nReg, int *espacoMax);

    // Remove um registro do vetor, sendo usada em conjunto com a funcionalidade 5
    bool indice_remover(REGISTROI *registros, int idBuscado, int tamanho);

    // Carrega os registros do arquivo de índices para um vetor na memória princiapl
    bool indice_carregamento(char *indice, char *binario,REGISTROI* registros);

    // Dado um arquivo inconcistente, ela o reescreve com os registros no vetor em memória principal
    // Importante para a inserção e remoção no binário com índice
    bool indice_reescrita(char *indice, REGISTROI *regs, int nRegs);

    // Desaloca o vetor de registros na memória principal
    bool indice_destruir(REGISTROI *registros);

#endif