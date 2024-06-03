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

    // Função que cria um VETREGISTROI vazio a partir de um tamanho inicial
    VETREGISTROI *indice_criar_vetor(int tamanho);


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
    bool indice_destruir(VETREGISTROI *registros);

#endif

/*
Pq não criar direto aqui:
PROBLEMA 1:
reader.c deve abrigar a interface de todas as funcionalidades, logo chamaremos 4 dele
Para criar aqui, teriamos que passar o vetor do regDados para ca, mas para lê-lo prcisariamos de le_registro_binario
Isso faria ela aparecer no reader.h, quebrando a lógica de usar reader como interface
Além disso, se quisermos que indice seja independente do binario, precisamos faze-lo ficar independete de regDados
Por fim, create_index precisaria de um vetregistroi para usar reescrita, mas o problmea 2 dificulta a criação dele la:

PROBLEMA 2:
Se VETREGADADOS em teoria deveria funcionar como um tad, para nenhuma força externa alterar espacoMax ou nREgs
Além disso, a cada realoc em carregamento e inserção precisamos verificar se o novo vetor não é nulo, mas isso
não há valor nulo para vetregistroi, pois ele não é um ponteiro, e não podemos fazer carregamento ser bool sem passar por referencia

ideia: indice_criar recebe o tamanho e o vetor de registroi de reader.c, mantendo a interface
Ela cria um VETREGISTROI e o usa para chamar a reescrita do arquivo binário, fazendo o indice.c ficar quase independente do reader.c
Depois a funcionalidade só precisa se preocupara em verificar o que ela retornar e desalocar tudo.

Sinceramente, as vezes eu desgosto do fato da parte de implementação dessa diciplina ser tão jogada...
*/