#include "util.h"

bool consistente(FILE *ptrArquivo) {
    if(ptrArquivo != NULL) {
        char status;
        fread(&status, sizeof(char), 1, ptrArquivo);
        if(status == '1')
            return true;
        }
    return false;
}

int get_numero_registros(FILE* arquivo){
    char aux;
    int n=0;

    while(fscanf(arquivo,"%c",&aux)!=EOF){
        if(aux==',')n++;
    }
    n=n/4;
    fseek(arquivo,45,SEEK_SET);
    //Alteração para o Fseek funcionar em windows e linux
    char c;
    fread(&c,sizeof(char),1,arquivo);
    if(c!='\n')fseek(arquivo,-1,SEEK_SET);
    return n-1;
}

bool set_status_arquivo(FILE *arquivo, char status) {
    if(arquivo != NULL || (status != '0' && status != '1'))
        return false;

    if(ftell(arquivo) != 0)
        fseek(arquivo, 0, SEEK_SET);

    fwrite(&status, sizeof(char), 1, arquivo);
    return true;
}

int strToInt(char *str, int tam) {
    if(tam == 0)
        return -1;

    if(str != NULL) {
        int n = 0;
        int casaDec = 1;

        for(int i = tam-1; i>=0; i--) {
            n += (str[i]-'0') * casaDec;
            casaDec *= 10;
        }

        return n;
    }
    return -1;
}

int get_tamanho_string(char *string){
    int ct=0;
    while(string[ct]!='\0'){
        ct++;
    }
    return ct;
}


//Auxiliar para o quick sort em vetor de registro de indices
int quickSortParticao(REGISTROI *vet, int ini, int fim) {
    int j = ini;    // Índice da próxima posição a ser trocada no quick sort
    REGISTROI pivo = vet[fim];
    REGISTROI aux;


    for(int i = ini; i < fim; i++) {
        if(vet[i].id < pivo.id) {
            // Trocando o elemento de i com j
            aux = vet[i];
            vet[i] = vet[j];
            vet[j] = aux;

            j++;
        }
    }

    // Realizando a troca no pivo
    vet[fim] = vet[j];
    vet[j] = pivo;
    return j;
}

void quickSort(REGISTROI *vet, int ini, int fim) {
    if(ini < fim) {
        // Ordenando a partição atual.
        int pivot = quickSortParticao(vet, ini, fim);

        // Chamadas recursivas para as partições
        quickSort(vet, ini, pivot - 1);
        quickSort(vet, pivot + 1, fim);
    }
    return;
}

void imprimeVetRegistroi(VETREGISTROI *vet) {
    int nReg = indice_get_nReg(vet);
    int espacoAlocado = indice_get_espacoMax(vet);
    if(nReg == -1 || espacoAlocado == -1)
        return;

    REGISTROI regi;

    printf("Registros lidos: %d\n", nReg);
    printf("Espaço alocado: %d\n\n", espacoAlocado);

    for(int i = 0; i < nReg; i++) {
        regi = indice_get_registroi_vetor(vet, i);
        printf("%d: %lld\n", regi.id, regi.byteOffset);
    }
    printf("\n\n");
}

void imprimeIndice(char *indice) {
    FILE *arquivo;
    REGISTROI reg;
    char status;

    arquivo = fopen(indice, "rb");
    if(!consistente(arquivo))
        return;

    while(!feof(arquivo)) {
        fread(&reg.id, sizeof(int), 1, arquivo);
        fread(&reg.byteOffset, sizeof(long long), 1, arquivo);

        printf("%d: %lld\n", reg.id, reg.byteOffset);
    }
    printf("\n");
}