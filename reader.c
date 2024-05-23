#include "reader.h"
#include "regCabecalho.h"
#include "funcoes_fornecidas.h"

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
    if(arquivo==NULL){return false;}

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
    if(arquivo == NULL) {return false;}

    // Escrevendo o registro de cabeçalho no binário
    escreve_cabecalho(arquivo, cabecalho);
    escreve_registro(arquivo, registros, n);

    fseek(arquivo, 0, SEEK_SET);
    cabecalho_set_status(cabecalho);
    escreve_cabecalho(arquivo,cabecalho);
    fclose(arquivo);


    cabecalho_apagar(&cabecalho);
    for(int i=0;i<n;i++){
        free(registros[i].nomeJogador);
        free(registros[i].nomeClube);
        free(registros[i].nacionalidade);
    }
    free(registros);
    registros = NULL;

    binarioNaTela(binario);

    return true;
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
    REGISTRO r;

    arquivo = fopen(binario, "rb");
    if(arquivo == NULL) {return false;}

    // Salvando o nroRegArq do cabeçalho em tamanho
    fseek(arquivo, 17, SEEK_SET);
    fread(&tamanho, sizeof(int), 1, arquivo);
    
    // Lendo e imprimindo todos os registros do arquivo, se houver
    if(tamanho == 0) {
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


bool reader_select_where(char * binario, int qntd) {
    FILE *arquivo;          // Ponteiro para o arquivo binário
    char campo[30];         // Campo que o usuário digitará para a busca
    int procurado[6];       // vetor para controle dos campos o usuário está procurando
    REGISTRO *regs;         // Vetor que armazena os registros encontrados na busca
    int tamanho;            // Variável auxiliar para indexar o vetor de registros por buscas
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

    // Alocando o vetor de registros
    fseek(arquivo, 17, SEEK_SET);
    fread(&tamanho, sizeof(int), 1, arquivo);

    // Alocando regs e qntdBuscas (e inicializando o último com 0)
    regs = (REGISTRO *) malloc(tamanho * sizeof(REGISTRO));
    qntdBuscas = (int *) malloc(qntd * sizeof(int));
    if(regs == NULL || qntdBuscas == NULL)
        return false;
    
    for(int i = 0; i < qntd; i++) qntdBuscas[i] = 0;
    tamanho = 0;    // Vai ser reutilizado para acessar as posições corretas do vetor de registros

    // Loop externo que executa a qntd de buscas informadas
    for(int i = 0; i < qntd; i++){
        int params;
        scanf("%d",&params);

        // Resetando todos os campos como não procurados pelo usuário na busca
        for(int j=1;j<6;j++)procurado[j] = 0;

        // Recebendo as condições do usuário para cada parametro ad busca atual
        for(int j=0;j<params;j++){
            scanf(" %s",campo);

            if(strcmp(campo,"id") == 0) {
                procurado[1]=1;
                scanf("%d",&id);
            }
            if(strcmp(campo,"idade") == 0) {
                procurado[2]=1;
                scanf("%d",&idade);
            }
            if(strcmp(campo,"nomeJogador") == 0) {
                procurado[3]=1;
                scan_quote_string(nomeJogador);
            }
            if(strcmp(campo,"nacionalidade") == 0) {
                procurado[4]=1;
                scan_quote_string(nacionalidade);
            }
            if(strcmp(campo,"nomeClube") == 0) {
                procurado[5]=1;
                scan_quote_string(nomeClube);
            }
        }

        // Percorrendo o arquivo para a busca atual
        fseek(arquivo,25,SEEK_SET);
        for(int j = 0; j < 1000; j++) {
            REGISTRO r = ler_registro_binario(arquivo);
            bool ok = true;

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
                regs[tamanho + qntdBuscas[i]] = r;
                qntdBuscas[i] += 1;

            }
            else 
                libera_registro(r);
            if(id_encontrado) {
                id_encontrado = 0;
                break;      // Parando a busca caso o id tenha sido encontrado
            }
        }
        tamanho += qntdBuscas[i];
    }

    // Imprimindo os resultados das buscas armazenadas no vetor
    tamanho = 0;
    for(int i = 0; i < qntd; i++) {
        printf("Busca %d\n\n", i + 1);

        if(qntdBuscas[i] > 0) {
            for(int j = 0; j < qntdBuscas[i]; j++) {
                imprime_registro(regs[tamanho + j]);
                libera_registro(regs[tamanho + j]);
            }
        }
        else
            printf("Registro inexistente.\n\n");
        tamanho += qntdBuscas[i];
    }

    free(qntdBuscas);
    free(regs);

    fclose(arquivo);
    return true;
}