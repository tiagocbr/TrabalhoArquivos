#include "indiceSimples.h"

struct vetRegistroi {
        REGISTROI *vet;
        int nReg;
        int espacoMax;
    };

REGISTROI indice_criar_registro(int id, long long offset) {
    REGISTROI novoReg;

    novoReg.id = id;
    novoReg.byteOffset = offset;
    return novoReg;
}

REGISTROI indice_get_registroi_vetor(VETREGISTROI *vet, int pos) {
    if(vet != NULL && pos < vet->nReg) 
        return vet->vet[pos];
    
    // Do contrário, retorna um REGISTROI com id indicando que é nulo (-1)
    REGISTROI regNulo;
    regNulo.id = -1;
    regNulo.byteOffset = -1;
    
    return regNulo;
}

VETREGISTROI *indice_criar_vetor(int tamanho) {
    VETREGISTROI *novoVetor;

    // Alocando memória para o tipo VETREGISTROI
    novoVetor = (VETREGISTROI *) malloc(sizeof(VETREGISTROI));
    if(novoVetor == NULL)
        return NULL;
    
    // Alocando memória para o vetor de registrosi interno
    novoVetor->vet = (REGISTROI *) malloc(tamanho * sizeof(REGISTROI));
    if (novoVetor->vet == NULL) {
        free(novoVetor);
        return NULL;
    }
    novoVetor->espacoMax = tamanho;
    novoVetor->nReg = 0;

    return novoVetor;
}

bool indice_criar(char *indice, REGISTROI *registros, int tamanho) {
    if(registros == NULL)
        return false;

    bool res;
    VETREGISTROI *vetRegistros;
    
    // Alocando memória para o vetor de registros e inicializando seus campos
    vetRegistros = (VETREGISTROI *) malloc(sizeof(VETREGISTROI));
    if(vetRegistros == NULL)
        return false;
    vetRegistros->vet = registros;
    vetRegistros->nReg = tamanho;
    vetRegistros->espacoMax = tamanho;

    // Chamando reescrita com vetRegistros para criar o arquivo de índices
    res = indice_reescrita(indice, vetRegistros);
    return res;
}

// Função auxiliar que verifica se um arquivo está consistente pelo seu status
// Como ela checa se o ponteiro é nulo, as funções que a usam logo depois de fopen
// Não precisam desse verificação.
bool consistente(FILE *ptrArquivo) {
    if(ptrArquivo != NULL) {
        char status;
        fread(&status, sizeof(char), 1, ptrArquivo);
        if(status == '1')
            return true;
        }
    return false;
}

int indice_buscar(VETREGISTROI *registros, int idBuscado) {
    if(registros->vet != NULL && registros->nReg > 0) {
        int meio = (registros->nReg - 1) / 2;   // índice para percorrer o vetor com busca binária
        int ini = 0;                            // Guarda o índice inicial da partição buscada a cada iteração
        int fim = registros->nReg - 1;          // Guarda o fim da partição buscada a cada iteração

        // Loop da busca binária
        while(ini <= fim) {
            if(registros->vet[meio].id == idBuscado)
                return meio;
            else if(registros->vet[meio].id > idBuscado)
                fim = meio - 1;
            else
                ini = meio + 1;

            meio = (fim + ini) / 2;
        }
    }
    return -1;
}

bool indice_inserir(VETREGISTROI *registros, REGISTROI novo) {
    if(registros->vet != NULL) {
        // Realocando mais posições na memória, caso necessário
        if(registros->nReg == registros->espacoMax) {
            registros->vet = (REGISTROI *) realloc(registros->vet, (2 * registros->nReg * sizeof(REGISTROI)));
            registros->espacoMax *= 2; 
        }

        int i; // Variável que indexará o veteor de registrosi

        // Buscando, deslocando e inserindo o novo registro ordenadamente
        for(i = 0; i < registros->nReg; i++) {
            if(registros->vet[i].id > novo.id) {
                // Caso a posição do novo registro, esteja ocupada por um removido, basta trocá-lo
                if(registros->vet[i].byteOffset == -1) {
                    registros->vet[i] = novo;
                    return true;    
                }

                // Caso contrátio, teremos que deslocar os registros para a direita
                for(int atual = registros->nReg; atual >= i; atual--) {
                    registros->vet[atual] = registros->vet[atual - 1];
                }
                break;
            }
        }

        // Inserção para os casos de deslocamento e de o registro ser inserido no final
        registros->vet[i] = novo;
        registros->nReg++;
        return true;   
    }
    return false;
}

bool indice_remover(VETREGISTROI *registros, int idBuscado) {
    if(registros->vet != NULL && registros->nReg > 0) {
        int pos;    // Variável que armazenará o índice do elemento a ser removido no vetor de registros

        // Buscando e removendo o item buscado
        // Para removê-lo, marcamos como -1 no byteoffset do registro, o que é usado para verificar se ele
        // está removido ou não em outras funções como reescrita e busca.
        pos = indice_buscar(registros, idBuscado);
        if(pos != -1)
            registros->vet[pos].byteOffset = -1;

        return true;
    }
    return false;
}

VETREGISTROI *indice_carregamento(char *indice, char *binario) {
    FILE *ptrIndice, *ptrBinario;   // Ponteiros para os arquivos
    VETREGISTROI *registros;        // Vetor de registrosi a ser retornado

    // Abrindo o arquivo de índice e verificando se ele está consistente.
    ptrIndice = fopen(indice, "rb");
    if(!consistente(ptrIndice))
        return NULL;

    // Abrindo o arquivo de dados para obter o número de registros
    ptrBinario = fopen(binario, "rb");
    if(!consistente(ptrBinario))
        return NULL;
    
    // Criando o vetor de registrosi
    registros = (VETREGISTROI *) malloc(sizeof(VETREGISTROI));
    if(registros == NULL)
        return NULL;

    fseek(ptrBinario, 17, SEEK_SET);
    fread(&registros->espacoMax, sizeof(int), 1, ptrBinario);
    registros->nReg = registros->espacoMax;
    fclose(ptrBinario);

    // Alocando memória e passando os valores do arquivo para o vetor
    registros->vet = (REGISTROI *) malloc(registros->espacoMax * sizeof(REGISTROI));
    if(registros->vet == NULL)
        return NULL;
    
    for(int i = 0; i < registros->nReg; i++) {
        fread(&registros->vet[i].id, sizeof(int), 1, ptrIndice);
        fread(&registros->vet[i].byteOffset, sizeof(long long), 1, ptrIndice);
    }

    fclose(ptrIndice);
    return registros;
}

bool indice_reescrita(char *indice, VETREGISTROI *regs) {
    char cabecalho = '0';   // Variável que guarda o valor do registro de cabeçalho
    FILE *arquivo;          // Ponteiro para o arquivo de índices

    // Verificando se o vetor de registros foi devidamente criado
    if(regs == NULL)
        return false;

    // Abrindo o arquivo de índices para escrita
    arquivo = fopen(indice, "wb");
    if(arquivo == NULL)
        return false;

    // Reescrevendo o status e os registros
    fwrite(&cabecalho, sizeof(char), 1, arquivo);
    for(int i = 0; i < regs->nReg; i++) {
        if(regs->vet[i].byteOffset != -1) {
            fwrite(&regs->vet[i].id, sizeof(int), 1, arquivo);
            fwrite(&regs->vet[i].byteOffset, sizeof(long long), 1, arquivo);
        }
    }

    // Se o loop terminou, podemos colocar o status do arquivo como consistente.
    cabecalho = '1';
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&cabecalho, sizeof(char), 1, arquivo);

    fclose(arquivo);
    return true;   
}

bool indice_destruir(VETREGISTROI *registros) {
    if(registros == NULL)
        return false;

    free(registros->vet);
    registros->vet = NULL;
    
    free(registros);
    registros = NULL;
    return true;
}



// Por enquanto ela serve apenas para debugar...
void imprimeVetRegistroi(VETREGISTROI *vet) {
    printf("Registros lidos: %d\n", vet->nReg);
    printf("Espaço alocado: %d\n\n", vet->espacoMax);

    for(int i = 0; i < vet->nReg; i++) {
        printf("%d: %lld\n", vet->vet[i].id, vet->vet[i].byteOffset);
    }
    printf("\n\n");
}

void imprimeIndice(char *indice) {
    FILE *arquivo;
    REGISTROI reg;
    char status;

    arquivo = fopen(indice, "rb");
    if(arquivo == NULL)
        return;
    
    fread(&status, sizeof(char), 1, arquivo);
    if(status == '0')
        return;

    while(!feof(arquivo)) {
        fread(&reg.id, sizeof(int), 1, arquivo);
        fread(&reg.byteOffset, sizeof(long long), 1, arquivo);

        printf("%d: %lld\n", reg.id, reg.byteOffset);
    }
    printf("\n");
}