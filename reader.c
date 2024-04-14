#include "reader.h"
#include "regCabecalho.h"

struct registro{
    char removido;
    int tamanhoRegistro;
    long long prox;
    int id;              //campo 1
    int idade;           //campo 2
    int tamNomeJog;
    char *nomeJogador;   //campo 3
    int tamNacionalidade;
    char *nacionalidade; //campo 4
    int tamNomeClube;
    char *nomeClube;     //campo 5
};

// Função auxiliar que percorre o csv para descobrir o seu número total de registros
int get_numero_registros(FILE* arquivo){
    char aux;
    int n=0;

    fseek(arquivo,45,SEEK_SET);

    while(fscanf(arquivo,"%c",&aux)!=EOF){
        if(aux==',')n++;
    }
    n=n/4;
    fseek(arquivo,45,SEEK_SET);
    return n;
}

// Função auxiliar que pega uma string lida no csv e a converte em inteiro
// (Usada para armazenar os campos id e idade)
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

// Função auxiliar que lê um determinado valor dos registros do csv e armazena
// Seu valor no respectivo campo do registro atual do vetor de registros.
void ler_campo(FILE *arquivo,int campo,int registro_atual,REGISTRO* registros, CABECALHO *cabecalho){
    char* valor;  // String dinâmica que armazena o campo lido
    int i=0;      // Index da string (usado para armazenar os caracteres do registro)
    int t=20;     // Tamanho da string (iniciada com 20)
    char aux;     // variável auxiliar que recebe um char do campo no csv para armazena-lo na string valor

    valor=(char*)malloc(sizeof(char)*20);

    // Loop que lê o campo char a char para armazená-lo em valor.
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

    // Escolhendo o campo correto da struct para o valor lido
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

// Função auxiliar que escreve no arquivo binário o registro de cabeçalho
void escreve_cabecalho(FILE *arquivo, CABECALHO *cabecalho) {
    if (cabecalho != NULL) {
        // Passando os dados do TAD para variáveis locais
        char status=cabecalho_get_status(cabecalho);
        long long topo =  cabecalho_get_topo(cabecalho);
        long long proxByteOffset = cabecalho_get_proxOffset(cabecalho);
        int nroRegArq = cabecalho_get_nroRegArq(cabecalho);
        int nroRegRem = cabecalho_get_nroRegRem(cabecalho);

        // Escrevendo o cabeçalho no arquivo
        fwrite(&status, sizeof(char),1,arquivo);
        fwrite(&topo, sizeof(long long),1,arquivo);
        fwrite(&proxByteOffset, sizeof(long long),1,arquivo);
        fwrite(&nroRegArq, sizeof(int),1,arquivo);
        fwrite(&nroRegRem, sizeof(int),1,arquivo);
    
    }
}

void escreve_registro(FILE *arquivo, REGISTRO *registros, int qntd) {
    if (registros != NULL) {
        for(int i = 0; i < qntd; i++) {
            // Escrevendo campos fixos
            fwrite(&registros[i].removido, sizeof(char), 1, arquivo);
            fwrite(&registros[i].tamanhoRegistro, sizeof(int), 1, arquivo);
            fwrite(&registros[i].prox, sizeof(long long), 1, arquivo);
            fwrite(&registros[i].id, sizeof(int), 1, arquivo);
            fwrite(&registros[i].idade, sizeof(int), 1, arquivo);

            // Escrevendo campos variáveis(se forem informados)
            fwrite(&registros[i].tamNomeJog, sizeof(int), 1, arquivo);
            if(registros[i].tamNomeJog != 0)
                fwrite(registros[i].nomeJogador, sizeof(char), registros[i].tamNomeJog, arquivo);

            fwrite(&registros[i].tamNacionalidade, sizeof(int), 1, arquivo);
            if(registros[i].tamNacionalidade != 0)
                fwrite(registros[i].nacionalidade, sizeof(char), registros[i].tamNacionalidade, arquivo);

            fwrite(&registros[i].tamNomeClube, sizeof(int), 1, arquivo);
            if(registros[i].tamNomeClube != 0)
                fwrite(registros[i].nomeClube, sizeof(char), registros[i].tamNomeClube, arquivo);
        }
    }
}

bool reader_create_table(char* csv,char* binario) {
    // Abrindo o arquivo csv para ler os dados
    // arquivo é o ponteiro usado para ler ambos arquivos
    FILE* arquivo=fopen(csv,"r");
    if(arquivo==NULL){printf("Teste1\n"); return false;}

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
        printf("%d\n",registros[registro_atual].id);
    }
    fclose(arquivo);

    // Criando o binário
    arquivo = fopen(binario, "wb"); 
    if(arquivo == NULL) {printf("Teste1\n"); return false;}

    // Escrevendo o registro de cabeçalho no binário
    escreve_cabecalho(arquivo, cabecalho);
    escreve_registro(arquivo, registros, n);

    fseek(arquivo, 0, SEEK_SET);
    cabecalho_set_status(cabecalho);
    escreve_cabecalho(arquivo,cabecalho);
    fclose(arquivo);

    for(int i=0;i<n;i++){
        free(registros[i].nomeJogador);
        free(registros[i].nomeClube);
        free(registros[i].nacionalidade);
    }
    free(registros);
    return true;
}

// Função auxiliar que, dado um ponteiro para FILE na posção inicial de um
// registro, lê e imprime seus valores
void imprime_registro(FILE *arquivo) {
    REGISTRO r;              // Recebe o registro da posição atual do arquivo para imprimí-lo
    char *nulo = "Sem dado"; // Constante char para caso um campo variável não tenha sido informado

    // Lendo do arquivo
    fread(&r.removido, sizeof(char), 1, arquivo);
    fread(&r.tamanhoRegistro, sizeof(int), 1, arquivo);
    fread(&r.prox, sizeof(long long), 1, arquivo);
    fread(&r.id, sizeof(int), 1, arquivo);
    fread(&r.idade, sizeof(int), 1, arquivo);

    fread(&r.tamNomeJog, sizeof(int), 1, arquivo);
    if(r.tamNomeJog == 0)
        r.nomeJogador = nulo;
    else {
        r.nomeJogador = (char *) malloc(r.tamNomeJog * sizeof(char));

        if(r.nomeJogador != NULL) {
            fread(r.nomeJogador, sizeof(char), r.tamNomeJog, arquivo);
            r.nomeJogador[r.tamNomeJog] = '\0';
        }
    }

    fread(&r.tamNacionalidade, sizeof(int), 1, arquivo);
    if(r.tamNacionalidade == 0)
        r.nacionalidade = nulo;
    else {
        r.nacionalidade = (char *) malloc(r.tamNacionalidade * sizeof(char));

        if(r.nacionalidade != NULL) {
            fread(r.nacionalidade, sizeof(char), r.tamNacionalidade, arquivo);
            r.nacionalidade[r.tamNacionalidade] = '\0';
        }
    }

    fread(&r.tamNomeClube, sizeof(int), 1, arquivo);
    if(r.tamNomeClube == 0)
        r.nomeClube = nulo;
    else {
        r.nomeClube = (char *) malloc(r.tamNomeClube * sizeof(char));

        if(r.nomeClube != NULL) {
            fread(r.nomeClube, sizeof(char), r.tamNomeClube, arquivo);
            r.nomeClube[r.tamNomeClube] = '\0';
        }
    }

    // Imprimindo
    printf("Removido: %c\n", r.removido);
    printf("Tamanho do registro: %d\n", r.tamanhoRegistro);
    printf("Próximo byte offset removido: %lld\n", r.prox);
    printf("Id: %d\n", r.id);
    printf("Idade: %d\n", r.idade);
    printf("Tamanho do nome do jogador: %d\n", r.tamNomeJog);
    printf("Nome do jogador: %s\n", r.nomeJogador);
    printf("Tamanho da nacionalidade do jogador: %d\n", r.tamNacionalidade);
    printf("Nacionalidade do jogador: %s\n", r.nacionalidade);
    printf("Tamanho do nome do clube do jogador: %d\n", r.tamNomeClube);
    printf("Nome do clube do jogador: %s\n\n", r.nomeClube);

    // Desalocando as strings
    if(r.nomeJogador != nulo) {
        free(r.nomeJogador);
        r.nomeJogador = NULL;
    }
    if(r.nacionalidade != nulo) {
        free(r.nacionalidade);
        r.nacionalidade = NULL;
    }
    if(r.nomeClube != nulo) {
        free(r.nomeClube);
        r.nomeClube = NULL;
    }
}

int reader_select_from(char *binario) {
    FILE *arquivo;
    int tamanho;

    arquivo = fopen(binario, "rb");
    if(arquivo == NULL) {return -1;}

    fseek(arquivo, 17, SEEK_SET);
    fread(&tamanho, sizeof(int), 1, arquivo);

    fseek(arquivo, 4, SEEK_CUR);
    for(int i = 0; i < tamanho; i++) {
        imprime_registro(arquivo);
    }

    fclose(arquivo);
    return 1;
}
