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
    fseek(arquivo,0,SEEK_SET);
    return n++;
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
            valor = (char *) realloc(valor,sizeof(char)*(2*i));
            t=2*t;
        }
    }

    valor[i]='\0';

    switch(campo){
        case 1: 
            registros[registro_atual].id=(int) valor;
            break;

        case 2: 
            registros[registro_atual].idade = (int) valor;
            break;

        case 3: 
            registros[registro_atual].nomeJogador = valor;
            registros[registro_atual].tamNomeJog = i;
            break;

        case 4: 
            registros[registro_atual].nacionalidade =  valor;
            registros[registro_atual].tamNacionalidade = i;
            break;
        case 5: 
            registros[registro_atual].nomeClube = valor;
            registros[registro_atual].tamNomeClube = i;
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
        for(int i=1;i<=5;i++){
            ler_campo(arquivo,i,registro_atual,registros);
        }
    }

    for(int i = 0; i < n; i++) {
        printf("%s\n", registros[i].nomeJogador);
    }    

    free(registros);
}


