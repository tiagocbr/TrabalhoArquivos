#include "indiceSimples.h"

REGISTROI indice_criar_registro(int id, long long offset) {
    REGISTROI novoReg;

    novoReg.id = id;
    novoReg.byteOffset = offset;
    return novoReg;
}

// Função auxiliar que verifica se um arquivo está concistente pelo seu status
// Como ela checa se o ponteiro é nulo, as funções que a usam logo depois de fopen
// Não precisam desse verificação.
bool concistente(FILE *ptrArquivo) {
    if(ptrArquivo != NULL) {
        char status;
        fread(&status, sizeof(char), 1, ptrArquivo);

        if(status == '1')
            return true;
        }
    return false;
}

int indice_buscar(REGISTROI *registros, int idBuscado, int tamanho) {
    if(registros != NULL && tamanho > 0) {
        int meio = (tamanho - 1) / 2;   // índice para percorrer o vetor com busca binária
        int ini = 0;                    // Guarda o índice inicial da partição buscada a cada iteração
        int fim = tamanho - 1;          // Guarda o fim da partição buscada a cada iteração

        // Loop da busca binária
        while(ini <= fim) {
            if(registros[meio].id == idBuscado)
                return meio;
            else if(registros[meio].id > idBuscado)
                fim = meio - 1;
            else
                ini = meio + 1;

            meio = (fim + ini) / 2;
        }
    }
    return -1;
}

bool indice_inserir(REGISTROI *registros, REGISTROI novo, int nReg, int *espacoMax) {
    if(registros != NULL) {
        // Realocando mais 20 posições na memória, caso necessário
        if(nReg == *espacoMax) {
            registros = (REGISTROI *) realloc(registros, ((20 + nReg) * sizeof(REGISTROI)));
            *espacoMax += 20; 
        }

        // Buscando, deslocando e inserindo o novo registro ordenadamente
        for(int i = 0; i < nReg; i++) {
            if(registros[i].id > novo.id) {
                for(int atual = nReg; atual >= i; atual--) {
                    registros[atual] = registros[atual - 1];
                }

                registros[i] = novo;
                return true;
            }
        }
    }
    return false;
}

bool indice_remover(REGISTROI *registros, int idBuscado, int tamanho) {
    if(registros != NULL && tamanho > 0) {
        int pos;    // Variável que armazenará o índice do elemento a ser removido no vetor de registros

        // Buscando e deslocando os elementos do vetor para remover o buscado
        pos = indice_buscar(registros, idBuscado, tamanho);
        while(pos != tamanho - 1) {
            registros[pos] = registros[pos + 1];
            pos++;
        }

        // Como o loop termina em tamanho - 1, precisamos dessa linha para o último elemento não ficar
        // Duplicado. -1 é o valor nulo para id
        registros[pos].id = -1;
        return true;
    }
    return false;
}

REGISTROI *indice_carregamento(char *indice, char *binario) {
    FILE *ptrIndice, *ptrBinario; // Ponteiros para os arquivos
    REGISTROI *registros;         // Vetor de registrosi a ser retornado
    int tamanho;                  // Tamanho do vetor de registros

    // Abrindo o arquivo de índice e verificando se ele está concistente.
    ptrIndice = fopen(indice, "rb");
    if(!concistente(ptrIndice))
        return NULL;

    // Abrindo o arquivo de dados para obter o número de registros
    ptrBinario = fopen(binario, "rb");
    if(!concistente(ptrBinario))
        return NULL;
    
    fseek(ptrBinario, 17, SEEK_SET);
    fread(&tamanho, sizeof(int), 1, ptrBinario);
    fclose(ptrBinario);

    // Alocando memória e passando os valores do arquivo para o vetor
    registros = (REGISTROI *) malloc(tamanho * sizeof(REGISTROI));
    if(registros == NULL)
        return NULL;
    
    for(int i = 0; i < tamanho; i++) {
        fread(&registros[i].id, sizeof(int), 1, ptrIndice);
        fread(&registros[i].byteOffset, sizeof(long long), 1, ptrIndice);
    }

    fclose(ptrIndice);
    return registros;
}

bool indice_reescrita(char *indice, REGISTROI *regs, int nRegs) {
    char cabecalho = '0';   // Variável que guarda o valor do registro de cabeçalho
    FILE *arquivo;          // Ponteiro para o arquivo de índices

    // Abrindo o arquivo de índices para escrita
    arquivo = fopen(indice, "wb");
    if(arquivo == NULL)
        return false;

    // Reescrevendo o status e os registros
    fwrite(&cabecalho, sizeof(char), 1, arquivo);
    for(int i = 0; i < nRegs; i++) {
        fwrite(&regs[i].id, sizeof(int), 1, arquivo);
        fwrite(&regs[i].byteOffset, sizeof(long long), 1, arquivo);
    }

    // Se o loop terminou, podemos colocar o status do arquivo como concistente.
    cabecalho = '1';
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&cabecalho, sizeof(char), 1, arquivo);

    fclose(arquivo);
    return true;   
}

bool indice_destruir(REGISTROI *registros) {
    if(registros == NULL)
        return false;

    free(registros);
    registros = NULL;
    return true;
}



// Por enquanto ela serve apenas para debugar...
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