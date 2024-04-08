#include "reader.h"

struct cabecalho{
    char status;
    long long topo;
    long long proxByteOffset;
    int nroRegArq;
    int nroRegRem;
};

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

void ler_campo(FILE *arquivo,int campo,int registro_atual,REGISTRO* registros){
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
            registros[registro_atual].id = i==0 ? -1 : (int) valor;
            free(valor);
            break;

        case 2: 
            registros[registro_atual].idade = i== 0 ? -1 : (int) valor;
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
            break;
    }

}
bool reader_create_table(char* csv,char* binario){
    FILE* arquivo=fopen(csv,"r");
    if(arquivo==NULL)return false;
    fseek(arquivo,46,SEEK_SET);
    int n=get_numero_registros(arquivo);

    REGISTRO* registros = (REGISTRO*)malloc(n*sizeof(REGISTRO));
    for(int registro_atual=0;registro_atual<n;registro_atual++){
        registros[registro_atual].removido = '0';
        registros[registro_atual].tamanhoRegistro = 33;
        registros[registro_atual].prox = -1;
        for(int i=1;i<=5;i++){
            ler_campo(arquivo,i,registro_atual,registros);
        }
    }

    for(int i=0;i<n;i++){
        free(registros[i].nomeJogador);
        free(registros[i].nomeClube);
        free(registros[i].nacionalidade);
    }
    free(registros);
    return true;
}


