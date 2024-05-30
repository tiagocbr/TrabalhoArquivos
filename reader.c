#include "reader.h"
#include "regCabecalho.h"
#include "funcoes_fornecidas.h"
#include "indiceSimples.h"

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
typedef struct offset_tamanho OT;
struct offset_tamanho{
    int tamanhoReg;
    long long offsetReg;
};
// Função auxiliar que percorre o csv para descobrir o seu número total de registros
int get_numero_registros(FILE* arquivo){
    char aux;
    int n=0;

    while(fscanf(arquivo,"%c",&aux)!=EOF){
        if(aux==',')n++;
    }
    n=n/4;
    fseek(arquivo,45,SEEK_SET);
    return n-1;
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

// Função auxiliar que, dado um ponteiro para FILE na posção inicial de um
// registro, lê e imprime seus valores
void imprime_registro(REGISTRO r) {
    char* nulo =  "SEM DADO";   // Valor a ser impresso para registros com campos variáveis
                                // do tipo string nulos.
    if(r.tamNomeJog == 0){
        r.nomeJogador = nulo;
    }
    if(r.tamNacionalidade == 0){
        r.nacionalidade = nulo;
    }
    if(r.tamNomeClube == 0){
        r.nomeClube = nulo;
    }

    // Imprimindo todos os registros não logicamente removidos
    if(r.removido == '0') {
        printf("Nome do Jogador: %s\n", r.nomeJogador);
        printf("Nacionalidade do Jogador: %s\n", r.nacionalidade);
        printf("Clube do Jogador: %s\n\n", r.nomeClube);
    }
}

//libera as regiões de memória para onde esse registro aponta em nomejogador,nacionalidade e nome
// clube
void libera_registro(REGISTRO r){ 
    if(r.tamNomeJog != 0) {
        free(r.nomeJogador);
        r.nomeJogador = NULL;
    }
    if(r.tamNacionalidade != 0) {
        free(r.nacionalidade);
        r.nacionalidade = NULL;
    }
    if(r.tamNomeClube != 0) {
        free(r.nomeClube);
        r.nomeClube = NULL;
    }

}

// Função auxiliar que lê um determinado valor dos registros do csv e armazena
// Seu valor no respectivo campo do registro atual do vetor de registros.
void ler_campo(FILE *arquivo,int campo,REGISTRO* registro, CABECALHO *cabecalho){
    char* valor;  // String dinâmica que armazena o campo lido
    int i=0;      // Index da string (usado para armazenar os caracteres do registro)
    int t=20;     // Tamanho da string (iniciada com 20)
    char aux;     // variável auxiliar que recebe um char do campo no csv para armazena-lo na string valor

    valor=(char*)malloc(sizeof(char)*20);

    // Loop que lê o campo char a char para armazená-lo em valor.
    while(fscanf(arquivo,"%c",&aux) != EOF) {
        if(aux==',' || aux=='\n' || aux == '\r'){
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
            (*registro).id = strToInt(valor, i);
            free(valor);
            break;

        case 2: 
            (*registro).idade = strToInt(valor, i);
            free(valor);
            break;

        case 3: 
            (*registro).nomeJogador = valor;
            (*registro).tamNomeJog = i;
            (*registro).tamanhoRegistro+=i;
            break;

        case 4: 
            (*registro).nacionalidade = valor;
            (*registro).tamNacionalidade = i;
            (*registro).tamanhoRegistro+=i;
            break;
        case 5: 
            (*registro).nomeClube = valor;
            (*registro).tamNomeClube = i;
            (*registro).tamanhoRegistro+=i;
            cabecalho_set_proxOffset(cabecalho , (*registro).tamanhoRegistro + cabecalho_get_proxOffset(cabecalho));
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

void escreve_registro(FILE *arquivo, REGISTRO* registro) {
    // Escrevendo campos fixos
    fwrite(&registro->removido, sizeof(char), 1, arquivo);
    fwrite(&registro->tamanhoRegistro, sizeof(int), 1, arquivo);
    fwrite(&registro->prox, sizeof(long long), 1, arquivo);
    fwrite(&registro->id, sizeof(int), 1, arquivo);
    fwrite(&registro->idade, sizeof(int), 1, arquivo);

    // Escrevendo campos variáveis(se forem informados)
    fwrite(&registro->tamNomeJog, sizeof(int), 1, arquivo);
    if(registro->tamNomeJog != 0)
        fwrite(registro->nomeJogador, sizeof(char), registro->tamNomeJog, arquivo);

    fwrite(&registro->tamNacionalidade, sizeof(int), 1, arquivo);
    if(registro->tamNacionalidade != 0)
        fwrite(registro->nacionalidade, sizeof(char), registro->tamNacionalidade, arquivo);

    fwrite(&registro->tamNomeClube, sizeof(int), 1, arquivo);
    if(registro->tamNomeClube != 0)
        fwrite(registro->nomeClube, sizeof(char), registro->tamNomeClube, arquivo);
}

bool reader_create_table(char* csv,char* binario) {
    // Abrindo o arquivo csv para ler os dados
    // arquivo é o ponteiro usado para ler ambos arquivos
    FILE* arquivo=fopen(csv,"r");
    if(arquivo==NULL){return false;}

    int n=get_numero_registros(arquivo);
    
    // Instanciando as structs do vetor registro de dados e do cabeçalho
    CABECALHO *cabecalho = cabecalho_criar();

    cabecalho_set_nroRegArq(cabecalho,n);

    // Criando o binário
    arquivo = fopen(binario, "wb"); 
    if(arquivo == NULL) {return false;}
    // Escrevendo o registro de cabeçalho no binário
    escreve_cabecalho(arquivo, cabecalho);

    // Lendo no csv cada registro e armazenando-o no binário
    for(int registro_atual=0;registro_atual<n;registro_atual++){
        REGISTRO r;
        r.removido = '0';
        r.tamanhoRegistro = 33;
        r.prox = -1;

        for(int i=1;i<=5;i++) {
            ler_campo(arquivo,i,&r, cabecalho);
        }
        escreve_registro(arquivo, &r);
        libera_registro(r);
    }
    fclose(arquivo);


    

    fseek(arquivo, 0, SEEK_SET);
    cabecalho_set_status(cabecalho);
    escreve_cabecalho(arquivo,cabecalho);
    fclose(arquivo);


    cabecalho_apagar(&cabecalho);

    binarioNaTela(binario);

    return true;
}

// Função auxiliar que le um registro do arquivo binário e o salva em uma variável da struct
// registro
REGISTRO ler_registro_binario(FILE *arquivo){
    REGISTRO r;     // Variável que armazena e retorna o registro lido

    // Lendo os campos fixos do registro
    fread(&r.removido, sizeof(char), 1, arquivo);
    fread(&r.tamanhoRegistro, sizeof(int), 1, arquivo);
    fread(&r.prox, sizeof(long long), 1, arquivo);
    fread(&r.id, sizeof(int), 1, arquivo);
    fread(&r.idade, sizeof(int), 1, arquivo);

    // Pulando os campos variáveis caso o registro esteja lógicamente removido
    if(r.removido == '1') {
        fread(&r.tamNomeJog, sizeof(int), 1, arquivo);
        fseek(arquivo, r.tamNomeJog, SEEK_CUR);
        fread(&r.tamNacionalidade, sizeof(int), 1, arquivo);
        fseek(arquivo, r.tamNacionalidade, SEEK_CUR);
        fread(&r.tamNomeClube, sizeof(int), 1, arquivo);
        fseek(arquivo, r.tamNomeClube, SEEK_CUR);

        r.nacionalidade = NULL;
        r.nomeJogador = NULL;
        r.nomeClube = NULL;
    }
    else {
        // Lendo e alocando os campos variávies, quando necessário
        fread(&r.tamNomeJog, sizeof(int), 1, arquivo);
        if(r.tamNomeJog == 0)
            r.nomeJogador = NULL;
        else {
            r.nomeJogador = (char *) malloc((r.tamNomeJog+1) * sizeof(char));

            if(r.nomeJogador != NULL) {
                fread(r.nomeJogador, sizeof(char), r.tamNomeJog, arquivo);
                r.nomeJogador[r.tamNomeJog] = '\0';
            }
        }

        fread(&r.tamNacionalidade, sizeof(int), 1, arquivo);
        if(r.tamNacionalidade == 0)
            r.nacionalidade = NULL;
        else {
            r.nacionalidade = (char *) malloc((r.tamNacionalidade+1) * sizeof(char));

            if(r.nacionalidade != NULL) {
                fread(r.nacionalidade, sizeof(char), r.tamNacionalidade, arquivo);
                r.nacionalidade[r.tamNacionalidade] = '\0';
            }
        }

        fread(&r.tamNomeClube, sizeof(int), 1, arquivo);
        if(r.tamNomeClube == 0)
            r.nomeClube = NULL;
        else {
            r.nomeClube = (char *) malloc((r.tamNomeClube+1) * sizeof(char));

            if(r.nomeClube != NULL) {
                fread(r.nomeClube, sizeof(char), r.tamNomeClube, arquivo);
                r.nomeClube[r.tamNomeClube] = '\0';
            }
        }
    }
    return r;
}

bool reader_select_from(char *binario) {
    FILE *arquivo;      // Ponteiro para o arquivo informado
    int tamanho;        // Armazena a quantidade de registros no arquivo binário
    char status;        // Recebe o status do arquivo, para avaliar se este está concistente
    REGISTRO r;

    arquivo = fopen(binario, "rb");
    if(arquivo == NULL) {return false;}

    // Verificando se o arquivo está concistnte
    fread(&status, sizeof(char), 1, arquivo);
     if(status == '0')
        return false;

     // Salvando o nroRegArq do cabeçalho em tamanho
    fseek(arquivo, 17, SEEK_SET);
    fread(&tamanho, sizeof(int), 1, arquivo);
    
    // Lendo e imprimindo todos os registros do arquivo, se houver
    if(tamanho == 0 || status == '0') {
        printf("Registro inexistente.\n\n");
    }
    else {
        fseek(arquivo, 4, SEEK_CUR);

        while(1) {
            r = ler_registro_binario(arquivo);
            if(!feof(arquivo)) {
                imprime_registro(r);
                libera_registro(r);
            }
            else {
                break;
            }
        }

        libera_registro(r);
    }

    fclose(arquivo);
    return true;
}


//função que atualiza a lista de removidos e cabeçalho com base no vetor regs que precisa estar ordenado em ordem crescente de tamanho
//essa ordenação diminuirá a complexidade de atualizar a lista ao final de todas as remoções
void atualiza_lista(FILE* arquivo,OT* regs,int qntd){
    //SETANDO AS VARIAVEIS
    int ponteiro_regs=0;
    long long offset_topo = 1;
    //atualizando o numero de registros no cabecalho
    fseek(arquivo,17,SEEK_SET);
    int nRegArq;
    fread(&nRegArq,sizeof(int),1,arquivo);
    nRegArq-=qntd;
    fseek(arquivo,-4,SEEK_CUR);
    fwrite(&nRegArq,sizeof(int),1,arquivo);
    //atualiza o numero de registros removidos no cabecalho
    int nroRegRem=0;
    fread(&nroRegRem,sizeof(int),1,arquivo);
    nroRegRem+=qntd;
    fseek(arquivo,-4,SEEK_CUR);
    fwrite(&nroRegRem,sizeof(int),1,arquivo);
    //começar a percorrer a lista a partir do primeiro elemento indicado pelo campo topo no cabeçalho
    fseek(arquivo,offset_topo,SEEK_SET);
    long long prox;
    fread(&prox,sizeof(long long),1,arquivo);
    long long offset_anterior = offset_topo;
    while(ponteiro_regs<qntd){
        //offset e tamanho do registro que quero inserir na lista de removidos
        long long offset_registro_removido = regs[ponteiro_regs].offsetReg;
        int tamanhoRegistro = regs[ponteiro_regs].tamanhoReg;
        if(prox==-1){ //se estiver no final da lista
            //coloca o elemento atual para apontar para o registro que quero inserir na lista de removidos
            fseek(arquivo,-8,SEEK_CUR);
            fwrite(&offset_registro_removido,sizeof(long long),1,arquivo);
            //coloca o proximo da lista como -1,pois sera inserida no final
            fseek(arquivo,offset_registro_removido+5,SEEK_SET);
            prox =-1;
            fwrite(&prox,sizeof(long long),1,arquivo);

            ponteiro_regs++;
            offset_anterior = offset_registro_removido+5;
        }
        else{
            //pula para o proximo elemento
            fseek(arquivo,prox+1,SEEK_SET);
            int tamanho_registro_atual=-1;
            long long offset_elemento_atual_da_lista = ftell(arquivo) - 1;
            fread(&tamanho_registro_atual,sizeof(int),1,arquivo);
            //se o proximo elemento tiver tamanho maior do que o que eu quero inserir
            if(tamanho_registro_atual>=tamanhoRegistro){
                //coloca o elemento removido para apontar para o registro atual
                //na proxima iteração o prox será igual a esse elemento novamente
                prox = offset_elemento_atual_da_lista;
                fseek(arquivo,offset_registro_removido+5,SEEK_SET);
                fwrite(&prox,sizeof(long long),1,arquivo);
                //coloca o elemento anterior para apontar para o registro removido
                fseek(arquivo,offset_anterior,SEEK_SET);
                fwrite(&offset_registro_removido,sizeof(long long),1,arquivo);
                ponteiro_regs++;
                offset_anterior = offset_registro_removido+5;
            }
            else{
                fread(&prox,sizeof(long long),1,arquivo);
            }  
        } 
    }    
}


//função que realiza as buscas no arquivo principal e printa
bool busca_no_binario(FILE* arquivo,REGISTRO registro_buscado,int *procurado){
    int id = registro_buscado.id;
    int idade = registro_buscado.idade;
    char* nomeJogador =  registro_buscado.nomeJogador;
    char* nacionalidade = registro_buscado.nacionalidade;
    char* nomeClube = registro_buscado.nomeClube;
    int n_registros;
    bool id_encontrado=false;
    bool ok=false;
    // Salvando o numero de registros do arquivo na variavel n_registros
    fseek(arquivo, 17, SEEK_SET);
    fread(&n_registros, sizeof(int), 1, arquivo);
    // Percorrendo o arquivo para a busca atual
    fseek(arquivo,25,SEEK_SET);
    long long offset_prox_registro = 25;
    long long offset_registro_removido = -1;
    int qntdBuscas = 0;
    for(int j = 0; j < n_registros; j++) {
        REGISTRO r = ler_registro_binario(arquivo);
        bool ok = true;
        offset_registro_removido = offset_prox_registro;
        offset_prox_registro+=r.tamanhoRegistro;
        for(int k = 1; k <= 5; k++) {
            if(procurado[k] == 0) continue;
            switch(k) {
                case 1: 
                    if(id!=r.id) 
                        ok=false;
                    else
                        id_encontrado = true;
                    break;
                case 2: 
                    if(idade!=r.idade)
                        ok=false;
                    break;
                case 3: 
                    if(r.nomeJogador == NULL || strcmp(nomeJogador,r.nomeJogador)!=0) ok=false;
                    break;
                case 4: 
                    if(r.nacionalidade == NULL || strcmp(nacionalidade,r.nacionalidade)!=0) ok=false;
                    break;
                case 5: 
                    if(r.nomeClube == NULL || strcmp(nomeClube,r.nomeClube)!=0) ok=false;
                    break;
            }
        }
        if(ok && r.removido == '0') {
            imprime_registro(r);
            libera_registro(r);
            ok=true;
        }
        else 
            libera_registro(r);
        if(id_encontrado) {
            return ok;     // Parando a busca caso o id tenha sido encontrado
        }
    }
    return ok;
    
}

bool reader_select_where(char * binario, int qntd) {
    FILE *arquivo;          // Ponteiro para o arquivo binário
    char campo[30];         // Campo que o usuário digitará para a busca
    int procurado[6];       // vetor para controle dos campos o usuário está procurando
    REGISTRO *regs;         // Vetor que armazena os registros encontrados na busca
    int tamanho;            // Variável auxiliar para indexar o vetor de registros por buscas
    char status;            // Variável usada para verificar se o arquivo buscado está concistente
    int *qntdBuscas;        // Vetor que armazena a quantidade de registros encontrados para cada busca
    bool id_encontrado = 0; // Variável auxiliar que para a busca caso a condição seja um id e este tenha sido encontrado

    // Variáveis que guardam as condições da busca para cada campo informadas pelo usuário
    int id;
    int idade;
    char nomeClube[100];
    char nacionalidade[100];
    char nomeJogador[100];

    // Abrindo e verificando a própria abertura do arquivo
    arquivo = fopen(binario, "rb");
    if(arquivo == NULL)
        return false;

    // Verificando se o arquivo está consistente
    fread(&status, sizeof(char), 1, arquivo);
     if(status == '0')
        return false;

    // pegando o tamanho do arquivo o vetor de registros
    fseek(arquivo, 17, SEEK_SET);
    fread(&tamanho, sizeof(int), 1, arquivo);


    // Loop externo que executa a qntd de buscas informadas
    for(int i = 0; i < qntd; i++){
        int params;
        scanf("%d",&params);

        // Resetando todos os campos como não procurados pelo usuário na busca
        for(int j=1;j<6;j++)procurado[j] = 0;

        // Recebendo as condições do usuário para cada parametro ad busca atual
        REGISTRO registro_buscado;
        for(int j=0;j<params;j++){
            scanf(" %s",campo);

            if(strcmp(campo,"id") == 0) {
                procurado[1]=1;
                scanf("%d",&registro_buscado.id);
            }
            if(strcmp(campo,"idade") == 0) {
                procurado[2]=1;
                scanf("%d",&registro_buscado.idade);
            }
            if(strcmp(campo,"nomeJogador") == 0) {
                procurado[3]=1;
                registro_buscado.nomeJogador = (char*) malloc (sizeof(char)*100);
                scan_quote_string(registro_buscado.nomeJogador);
            }
            if(strcmp(campo,"nacionalidade") == 0) {
                procurado[4]=1;
                registro_buscado.nacionalidade = (char*) malloc (sizeof(char)*100);
                scan_quote_string(registro_buscado.nacionalidade);
            }
            if(strcmp(campo,"nomeClube") == 0) {
                procurado[5]=1;
                registro_buscado.nomeClube = (char*) malloc (sizeof(char)*100);
                scan_quote_string(registro_buscado.nomeClube);
            }
        }
        //Buscando e printando
        printf("Busca %d\n\n", i + 1);
        //essa função printa busca e jah printa os registros encontrados,retornando false caso nenhum seja encontrado
        if(busca_no_binario(arquivo,registro_buscado,procurado));
        else printf("Registro inexistente.\n\n");
        libera_registro(registro_buscado);
    }

    fclose(arquivo);
    return true;
}

//Quick sort em vetor de registro de indices
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
//quick sort em vetor de inteiros
int quickSortParticao_int(int *vet, int ini, int fim) {
    int j = ini;    // Índice da próxima posição a ser trocada no quick sort
    int pivo = vet[fim];
    int aux;


    for(int i = ini; i < fim; i++) {
        if(vet[i] < pivo) {
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

void quickSort_int(int *vet, int ini, int fim) {
    if(ini < fim) {
        // Ordenando a partição atual.
        int pivot = quickSortParticao_int(vet, ini, fim);

        // Chamadas recursivas para as partições
        quickSort_int(vet, ini, pivot - 1);
        quickSort_int(vet, pivot + 1, fim);
    }
    return;
}
//quick sort em vetor de OT
int quickSortParticao_ot(OT *vet, int ini, int fim) {
    int j = ini;    // Índice da próxima posição a ser trocada no quick sort
    OT pivo = vet[fim];
    OT aux;


    for(int i = ini; i < fim; i++) {
        if(vet[i].tamanhoReg < pivo.tamanhoReg) {
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

void quickSort_ot(OT *vet, int ini, int fim) {
    if(ini < fim) {
        // Ordenando a partição atual.
        int pivot = quickSortParticao_ot(vet, ini, fim);

        // Chamadas recursivas para as partições
        quickSort_ot(vet, ini, pivot - 1);
        quickSort_ot(vet, pivot + 1, fim);
    }
    return;
}

bool reader_create_index(char *binario, char *indice) {
    FILE *arquivo;             // Ponteiro para acessar os arquivos
    REGISTRO regDados;         // Variável que armazena um registro de dados do índice
    REGISTROI *registrosi;     // Vetor de registros do índice
    char status;               // Variável auxiliar que recebe o status do binário, para avaliar a sua concistencia
    int tamanho;               // Variável auxiliar que recebe o número de registros no binário original
    long long offsetReg = 25;  // Guarda o offset do registro atual no binário para salvá-lo no índice
    bool res;                  // Recebe o resultado da função indice reescrita, para o retorno desta função
    int i = 0;                 // Index para o vetor de registrosi

    // Abrindo e verificando o arquivo binário para leitura
    arquivo = fopen(binario, "rb");
    if(arquivo == NULL)
        return false;
    
    // Verficando se o binário aberto está concistente
    fread(&status, sizeof(char), 1, arquivo);
    if(status == '0')
        return false;

    // Criando o vetor de registros do índice com o número de registros do arquivo
    fseek(arquivo, 17, SEEK_SET);
    fread(&tamanho, sizeof(int), 1, arquivo);
    registrosi = (REGISTROI *) malloc(sizeof(REGISTROI) * tamanho);
    if(registrosi == NULL)
        return false;

    // Percorrendo o binário, criando os regsitros no vetor de registros do índice e o ordenando ao final
    fseek(arquivo, 4, SEEK_CUR);
    while(!feof(arquivo)) {
        regDados = ler_registro_binario(arquivo);

        if((regDados.removido != '1') && !feof(arquivo)) {
            registrosi[i] = indice_criar_registro(regDados.id, offsetReg);
            i++;
        }

        // Atualizando o offset para o próximo registro
        offsetReg += (long long) regDados.tamanhoRegistro;
    }
    quickSort(registrosi, 0, tamanho - 1);

    // Fechando o arquivo binário e abrindo o arquivo de índices para a escrita
    fclose(arquivo);
    res = indice_reescrita(indice, registrosi, tamanho);
    if(res)
        binarioNaTela(indice);

    // Desalocando a memória e retorno da funcionalidade
    free(registrosi);
    registrosi = NULL;
    return res;
}

bool busca_binaria(int id,int ini,int fim,REGISTROI* arr,long long *offset_removido){
    while(ini<=fim){
        int meio = (ini+fim)/2;
        if(arr[meio].id==id){
            *offset_removido = arr[meio].byteOffset;
            return true;
        }
        else if(arr[meio].id<id)ini=meio+1;
        else fim = meio-1;
    }
    return false;
  
}

//guarda o offset e o id dos registros encontrados e marca como removidos
int busca_para_remover(FILE* arquivo,REGISTRO registro_buscado,OT *regs,int*id_regs,int ini,int *procurado){
    int id = registro_buscado.id;
    int idade = registro_buscado.idade;
    char* nomeJogador =  registro_buscado.nomeJogador;
    char* nacionalidade = registro_buscado.nacionalidade;
    char* nomeClube = registro_buscado.nomeClube;
    int n_registros;
    bool id_encontrado=false;
    // Salvando o numero de registros do arquivo na variavel n_registros
    fseek(arquivo, 17, SEEK_SET);
    fread(&n_registros, sizeof(int), 1, arquivo);
    // Percorrendo o arquivo para a busca atual
    fseek(arquivo,25,SEEK_SET);
    long long offset_prox_registro = 25;
    long long offset_registro_removido = -1;
    int qntdBuscas = 0;
    for(int j = 0; j < n_registros; j++) {
        REGISTRO r = ler_registro_binario(arquivo);
        bool ok = true;
        offset_registro_removido = offset_prox_registro;
        offset_prox_registro+=r.tamanhoRegistro;
        for(int k = 1; k <= 5; k++) {
            if(procurado[k] == 0) continue;
            switch(k) {
                case 1: 
                    if(id!=r.id) 
                        ok=false;
                    else
                        id_encontrado = true;
                    break;
                case 2: 
                    if(idade!=r.idade)
                        ok=false;
                    break;
                case 3: 
                    if(r.nomeJogador == NULL || strcmp(nomeJogador,r.nomeJogador)!=0) ok=false;
                    break;
                case 4: 
                    if(r.nacionalidade == NULL || strcmp(nacionalidade,r.nacionalidade)!=0) ok=false;
                    break;
                case 5: 
                    if(r.nomeClube == NULL || strcmp(nomeClube,r.nomeClube)!=0) ok=false;
                    break;
            }
        }
        if(ok && r.removido == '0') {
            regs[ini + qntdBuscas].offsetReg = ftell(arquivo) - r.tamanhoRegistro; //offset para atualizar a lista depois
            id_regs[ini + qntdBuscas] = r.id; //id para atualizar o registros de indices depois
            regs[ini+qntdBuscas].tamanhoReg = r.tamanhoRegistro; //tamanho para atualizar a lista de removidos de maneira mais otimizada
            qntdBuscas += 1;
            //marcando o arquivo como removido
            fseek(arquivo,offset_registro_removido,SEEK_SET);
            char remov='1';
            fwrite(&remov,sizeof(char),1,arquivo);
            fseek(arquivo,offset_prox_registro,SEEK_SET);
            libera_registro(r);
        }
        else 
            libera_registro(r);
        if(id_encontrado) {
            return qntdBuscas;     // Parando a busca caso o id tenha sido encontrado
        }
    }
    return qntdBuscas;
}

bool reader_delete_where(char *binario,char *indice,int n){
    FILE* arquivo = fopen(binario,"wb+");
    int n_registros;
    //fazendo as verificações de status
    char status;
    fread(&status,sizeof(char),1,arquivo);
    if(status=='0'){
        return 0;
    }
    else{
        fseek(arquivo,-1,SEEK_CUR);
        status='0';
        fwrite(&status,sizeof(char),1,arquivo);
    }

    fseek(arquivo, 17, SEEK_SET);
    fread(&n_registros, sizeof(int), 1, arquivo);
     //ler o arquivo de indices inteiro e trazer para a ram no vetor vetor_indices
    REGISTROI *vetor_indices = indice_carregamento(indice, binario);
    if(vetor_indices == NULL)
        return 0;

    int procurado[6];
    char campo[20];
    int busca_total=0;

    OT *regs= (OT*) malloc(sizeof(OT)*n_registros);
    int *id_regs = (int*)malloc(sizeof(int)*n_registros);
    for(int i = 0; i < n; i++){
        int params;
        scanf("%d",&params);

        // Resetando todos os campos como não procurados pelo usuário na busca
        for(int j=1;j<6;j++)procurado[j] = 0;
        REGISTRO registro_buscado;
        // Recebendo as condições do usuário para cada parametro da busca atual
        for(int j=0;j<params;j++){
            scanf(" %s",campo);

            if(strcmp(campo,"id") == 0) {
                procurado[1]=1;
                scanf("%d",&registro_buscado.id);
            }
            if(strcmp(campo,"idade") == 0) {
                procurado[2]=1;
                scanf("%d",&registro_buscado.idade);
            }
            if(strcmp(campo,"nomeJogador") == 0) {
                procurado[3]=1;
                registro_buscado.nomeJogador = (char*)malloc(sizeof(char)*100);
                scan_quote_string(registro_buscado.nomeJogador);
            }
            if(strcmp(campo,"nacionalidade") == 0) {
                procurado[4]=1;
                registro_buscado.nacionalidade = (char*)malloc(sizeof(char)*100);
                scan_quote_string(registro_buscado.nacionalidade);
            }
            if(strcmp(campo,"nomeClube") == 0) {
                procurado[5]=1;
                registro_buscado.nomeClube = (char*)malloc(sizeof(char)*100);
                scan_quote_string(registro_buscado.nomeClube);
            }
        }
    
        if(procurado[1]==0){
            //busca no arquivo principal e guarda os offsets ,o id e o tamanho dos registros encontrados,além de os marcarem como removidos no arquivo principal
            //o id sera usado para atualizar o arquivo de indices e o offset/tamanho para atualizar a lista de removidos
            //guardamos os arquivos removidos em RAM pois isso ira diminuir a complexidade de atualizar a lista de removidos ao final
            //pois cada atualização da lista de removidos seria feita em até o(n )
            //no entanto se guardarmos todos os removidos e ordenálos por ordem de tamanho para inserir na lista, a inserção de todos sera em complexidade o(n )
            // O que é mais eficiente do que o(n ) para cada.
            int busca_atual = busca_para_remover(arquivo,registro_buscado,regs,id_regs,busca_total,procurado);
            busca_total+=busca_atual;
        }
        else{
            int id=registro_buscado.id;
            long long offset_removido;
            //faz a busca binaria no vetor_indices e guarda o resultado em offset_removido
            //analogo a busca_para_remover so que usando o arquivo de indices
            bool ok = busca_binaria(id,0,n_registros-1,vetor_indices,&offset_removido);
            if(ok){ //marca o registro como removido no arquivo principal e guarda o id/offset/tamanho
                fseek(arquivo,offset_removido,SEEK_SET);
                char removido;
                fread(&removido,sizeof(char),1,arquivo);
                if(removido=='1')continue; // se o registro ja esta removido nao precisa fazer nada
                fseek(arquivo,-1,SEEK_CUR);
                removido='1';
                fwrite(&removido,sizeof(char),1,arquivo);
                int tamanho_reg_rem;
                fread(&tamanho_reg_rem,sizeof(int),1,arquivo);
                //adiciona no regs para atualizar a lista de removidos e o arquivo de indices epois
                regs[busca_total].offsetReg = offset_removido;
                id_regs[busca_total] = id;
                regs[busca_total].tamanhoReg = tamanho_reg_rem;
                busca_total+=1;
            }
        }

        //dando free nos campos do registro buscado
        libera_registro(registro_buscado);
    }
    //removendo no arquivo principal os registros encontrados nas buscas

    //Fazendo um novo vetor para colocar no arquivo de indices
    REGISTROI *vetor_apos_remocao = (REGISTROI *) malloc (sizeof(REGISTROI)*(n_registros-busca_total));
    int ponteiro_vetor_apos_remocao=0;
    int ponteiro_regs=0;
    //quick sort no vetor id_regs
    quickSort_int(id_regs, 0, busca_total-1);
    for(int i=0;i<n_registros;i++){
        REGISTROI u = vetor_indices[i];
        if(ponteiro_regs<busca_total)
            if(u.id == id_regs[ponteiro_regs]){
                ponteiro_regs++;
                continue;
            }
        vetor_apos_remocao[ponteiro_vetor_apos_remocao] = u;
        ponteiro_vetor_apos_remocao++;
    }
    //reescrever o arquivo de indices a partir do vetor vetor_apos_remocao
    indice_reescrita(indice, vetor_apos_remocao,n_registros-busca_total);
    //quick sort em regs
    quickSort_ot(regs,0,busca_total-1);
    //reorganizar a lista de removidos a partir do vetor regs ordenado
    atualiza_lista(arquivo,regs,busca_total);

    //liberando memoria
    free(regs);
    free(id_regs);
    free(vetor_indices);
    free(vetor_apos_remocao);


    //fechando arquivo
    fseek(arquivo,0,SEEK_SET);
    status='1';
    fwrite(&status,sizeof(char),1,arquivo);
    fclose(arquivo);
    return true;
}

void inserir_arquivo_principal(FILE* arquivo,REGISTRO* regs,int qntd){
    int reaproveitados=0;//variavel para guardar quantos registros ocuparam lugar de registros logicamente removidos
    int ponteiro_regs=0;
    fseek(arquivo,1,SEEK_SET);
    int prox;
    fread(&prox,sizeof(int),1,arquivo);
    int offset_anterior = 1;
    while(ponteiro_regs<qntd){
        if(prox==-1){//inserir o resto no final do arquivo;
            fseek(arquivo,0,SEEK_END);
            for(int i=ponteiro_regs;i<qntd;i++)escreve_registro(arquivo,&regs[i]);
            break;
        }
        else{
            fseek(arquivo,prox+1,SEEK_SET);
            int tamanho;
            fread(&tamanho,sizeof(int),1,arquivo);
            fread(&prox,sizeof(int),1,arquivo);
            if(tamanho>=regs[ponteiro_regs].tamanhoRegistro){
                reaproveitados++;
                fseek(arquivo,-12,SEEK_CUR);
                regs[ponteiro_regs].tamanhoRegistro = tamanho;
                escreve_registro(arquivo,&regs[ponteiro_regs]);
                char lixo = '$';
                fwrite(&lixo,sizeof(char),tamanho-regs[ponteiro_regs].tamanhoRegistro,arquivo);
                ponteiro_regs++;
                fseek(arquivo,offset_anterior,SEEK_SET);
                fwrite(&prox,sizeof(int),1,arquivo);
            }
            else{
                offset_anterior = ftell(arquivo) - 8;
            }
        }
    }
    //atualizando os valores de n_registros e n_registros removidos no cabecalho
    fseek(arquivo,17,SEEK_SET);
    int n_reg;
    fread(&n_reg,sizeof(int),1,arquivo);
    n_reg+=qntd;
    fseek(arquivo,-4,SEEK_CUR);
    fwrite(&n_reg,sizeof(int),1,arquivo);
    fread(&n_reg,sizeof(int),1,arquivo);
    n_reg-=reaproveitados;
    fseek(arquivo,-4,SEEK_CUR);
    fwrite(&n_reg,sizeof(int),1,arquivo);
}

int get_tamanho_string(char *string){
    int ct=0;
    while(string[ct]!='\0'){
        ct++;
    }
    return ct;
}
void reader_insert_into(char *binario,char *indices,int n){
    REGISTRO* regs=(REGISTRO*)malloc(sizeof(REGISTRO)*n);
    for(int i=0;i<n;i++){
        scanf("%d %d",&regs[i].id,&regs[i].idade);
        regs[i].nomeJogador = (char*) malloc (sizeof(char)*100);
        regs[i].nacionalidade = (char*) malloc(sizeof(char)*100);
        regs[i].nomeClube = (char*) malloc(sizeof(char)*100);
        regs[i].removido = '0';
        regs[i].prox = -1;
        scan_quote_string(regs[i].nomeJogador);
        scan_quote_string(regs[i].nacionalidade);
        scan_quote_string(regs[i].nomeClube);
        regs[i].tamNomeJog = get_tamanho_string(regs[i].nomeJogador);
        regs[i].tamNacionalidade = get_tamanho_string(regs[i].nacionalidade);
        regs[i].tamNomeClube = get_tamanho_string(regs[i].nomeClube);
        regs[i].tamanhoRegistro = 33 + regs[i].tamNomeJog + regs[i].tamNacionalidade + regs[i].tamNomeClube;

    }
    FILE* arquivo = fopen(binario,"wb+");
    char status;
    fread(&status,sizeof(char),1,arquivo);
    if(status=='0'){
        printf("Falha no processamento do arquivo.");
        return;
    }
    status='1';
    fseek(arquivo,-1,SEEK_CUR);
    fwrite(&status,sizeof(char),1,arquivo);
    inserir_arquivo_principal(arquivo,regs,n);
    //inserir_no_arquivo_de_inices
    fclose(arquivo);

}