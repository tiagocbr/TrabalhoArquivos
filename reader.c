#include "reader.h"
#include "regCabecalho.h"

struct registro{
    char removido;
    int tamanhoRegistro;
    long long prox;
    int id; //campo 1
    int idade; //campo 2
    int tamNomeJog;
    char *nomeJogador; //campo 3
    int tamNacionalidade;
    char *nacionalidade; //campo 4
    int tamNomeClube;
    char *nomeClube; //campo 5
};

int get_numero_registros(FILE* arquivo){
    char aux;
    int n=0;
    while(fscanf(arquivo,"%c",&aux)!=EOF){
        if(aux==',')n++;
    }
    n=n/4;
    fseek(arquivo,46,SEEK_SET);
    return n;
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

void ler_campo(FILE *arquivo,int campo,int registro_atual,REGISTRO* registros, CABECALHO *cabecalho){
    char* valor=(char*)malloc(sizeof(char)*20);
    int i=0;
    int t=20;
    char aux;

    while(fscanf(arquivo,"%c",&aux) != EOF) {
        if(aux==',' || aux=='\n'){
            break;
        }
        valor[i]=aux;
        i++;
        if(i==t){
            valor = (char*)realloc(valor,sizeof(char)*(2*i));
            t=2*t;
        }
    }

    switch(campo){
        case 1: 
            registros[registro_atual].id = strToInt(valor, i);
            free(valor);
            break;

        case 2: 
            registros[registro_atual].idade = strToInt(valor, i);
            free(valor);
            break;

        case 3: 
            registros[registro_atual].nomeJogador = valor;
            registros[registro_atual].tamNomeJog = i;
            registros[registro_atual].tamanhoRegistro+=i;
            break;

        case 4: 
            registros[registro_atual].nacionalidade = valor;
            registros[registro_atual].tamNacionalidade = i;
            registros[registro_atual].tamanhoRegistro+=i;
            break;
        case 5: 
            registros[registro_atual].nomeClube = valor;
            registros[registro_atual].tamNomeClube = i;
            registros[registro_atual].tamanhoRegistro+=i;
            cabecalho_set_proxOffset(cabecalho , registros[registro_atual].tamanhoRegistro + cabecalho_get_proxOffset(cabecalho));
            break;
    }


}

void escreve_cabecalho(FILE *arquivo, CABECALHO *cabecalho) {
    if (cabecalho != NULL) {
        char status=cabecalho_get_status(cabecalho);
        long long topo =  cabecalho_get_topo(cabecalho);
        long long proxByteOffset = cabecalho_get_proxOffset(cabecalho);
        int nroRegArq = cabecalho_get_nroRegArq(cabecalho);
        int nroRegRem = cabecalho_get_nroRegRem(cabecalho);
        fwrite(&status, sizeof(char),1,arquivo);
        fwrite(&topo, sizeof(long long),1,arquivo);
        fwrite(&proxByteOffset, sizeof(long long),1,arquivo);
        fwrite(&nroRegArq, sizeof(int),1,arquivo);
        fwrite(&nroRegRem, sizeof(int),1,arquivo);
    
    }
}

bool reader_create_table(char* csv,char* binario) {
    // Abrindo o arquivo csv para ler os dados
    // arquivo é o ponteiro usado para ler ambos arquivos
    FILE* arquivo=fopen(csv,"r");
    if(arquivo==NULL)return false;

    fseek(arquivo,46,SEEK_SET);
    int n=get_numero_registros(arquivo);
    
    // Instanciando as structs do vetor registro de dados e do cabeçalho
    CABECALHO *cabecalho = cabecalho_criar();
    REGISTRO* registros = (REGISTRO*)malloc(n*sizeof(REGISTRO));

    cabecalho_set_nroRegArq(cabecalho,n);

    // Lendo no csv cada registro e armazenando-o no vetor
    for(int registro_atual=0;registro_atual<n;registro_atual++){
        registros[registro_atual].removido = '0';
        registros[registro_atual].tamanhoRegistro = 33;
        registros[registro_atual].prox = -1;

        for(int i=1;i<=5;i++) {
            ler_campo(arquivo,i,registro_atual,registros, cabecalho);
        }
    }
    fclose(arquivo);

    // Criando o binário
    arquivo = fopen(binario, "wb"); 
    if(arquivo == NULL) return false;

    // Escrevendo o registro de cabeçalho no binário
    escreve_cabecalho(arquivo, cabecalho);
    for(int i = 0; i < n; i++) {escreve_registro(arquivo, registros);}

    for(int i=0;i<n;i++){
        free(registros[i].nomeJogador);
        free(registros[i].nomeClube);
        free(registros[i].nacionalidade);
    }
    free(registros);
    return true;
}


