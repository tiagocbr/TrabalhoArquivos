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

    // Implementa um tipo para o vetor de registros carregado na memória, contendo
    // variáveis para controlar seu número de registros e o espaço alocado
    typedef struct vetRegistroi VETREGISTROI;

    // AUXILIARES PARA OPERAÇÕES EM CÓDIGOS EXTERNOS
    // Função auxiliar para criar na memória principal um registro do arquivo de ínidce
    // Apartir de um registro de dados lido
    REGISTROI indice_criar_registro(int id, long long offset);

    // Função que cria um VETREGISTROI a partir de um tamanho inicial
    // Todos os seus elementos são inicializados como um registroi nulo
    VETREGISTROI *indice_criar_vetor(int tamanho);

    //Conjunto de funções que retornam os valores de um VETREGISTROI
    int indice_get_nReg(VETREGISTROI *registros);
    int indice_get_espacoMax(VETREGISTROI *registros);
    REGISTROI indice_get_registroi_vetor(VETREGISTROI *vet, int pos);


    // FUNÇÕES PARA MANIPULAR O VETOR DO ÍNDICE SIMPLES NA MEMÓRIA
    // Cria o arquivo binário de índices a partir de um vetor de registrosi fornecido pelo arquivo principal
    bool indice_criar(char *indice, REGISTROI *registros, int tamanho);

    // Realiza uma busca binária a partir de um id fornecido, retornando
    // O valor do índice no vetor de registros
    int indice_buscar(VETREGISTROI *registros, int idBuscado);

    // Insere ordenadamente um novo registro no vetor, sendo usada com a funcionalidade 6
    bool indice_inserir(VETREGISTROI *registros, REGISTROI novo);

    // Remove um registro do vetor, sendo usada em conjunto com a funcionalidade 5
    bool indice_remover(VETREGISTROI *registros, int idBuscado);

    // Carrega os registros do arquivo de índices para um vetor na memória princiapl
    VETREGISTROI *indice_carregamento(char *indice, char *binario);

    // Dado um arquivo inconcistente, ela o reescreve com os registros no vetor em memória principal
    // Importante para a inserção e remoção no binário com índice
    bool indice_reescrita(char *indice, VETREGISTROI *regs);

    // Desaloca o vetor de registros na memória principal
    bool indice_destruir(VETREGISTROI **registros);

#endif