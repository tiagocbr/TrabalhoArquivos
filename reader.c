#include "util.h"
#include "reader.h"
#include "regCabecalho.h"
#include "funcoes_fornecidas.h"
#include "indiceSimples.h"
#include "arvore_b.h"

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
            if(aux=='\r')fscanf(arquivo,"%c",&aux);
            break;
        }
        valor[i]=aux;
        i++;
        if(i==t){
            valor = (char*)realloc(valor,sizeof(char)*(2*i));
            t=2*t;
        }
    }

    if(i == 0) {
        free(valor);
        valor = NULL;
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
    FILE* arquivo_bin = fopen(binario, "wb"); 
    if(arquivo == NULL) {return false;}
    // Escrevendo o registro de cabeçalho no binário
    escreve_cabecalho(arquivo_bin, cabecalho);

    long long proxOffset = 25;
    // Lendo no csv cada registro e armazenando-o no binário
    for(int registro_atual=0;registro_atual<n;registro_atual++){
        REGISTRO r;
        r.removido = '0';
        r.tamanhoRegistro = 33;
        r.prox = -1;

        for(int i=1;i<=5;i++) {
            ler_campo(arquivo,i,&r, cabecalho);
        }
        escreve_registro(arquivo_bin, &r);
        libera_registro(r);
        if(registro_atual == n-1)proxOffset = ftell(arquivo_bin);
    }
    fclose(arquivo);

    cabecalho_set_status(cabecalho);
    cabecalho_set_proxOffset(cabecalho,proxOffset);
    escreve_cabecalho(arquivo_bin,cabecalho);

    fclose(arquivo_bin);

    cabecalho_apagar(&cabecalho);

    binarioNaTela(binario);

    return true;
}

// Função auxiliar que le um registro do arquivo binário e o salva em uma variável da struct
// registro
REGISTRO ler_registro_binario(FILE *arquivo){
    REGISTRO r;     // Variável que armazena e retorna o registro lido
    long long ini = ftell(arquivo);
    // Lendo os campos fixos do registro
    fread(&r.removido, sizeof(char), 1, arquivo);
    fread(&r.tamanhoRegistro, sizeof(int), 1, arquivo);
    fread(&r.prox, sizeof(long long), 1, arquivo);
    fread(&r.id, sizeof(int), 1, arquivo);
    fread(&r.idade, sizeof(int), 1, arquivo);
    long long prox = ini+r.tamanhoRegistro;

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
    
    fseek(arquivo,prox,SEEK_SET);
    return r;
}

bool reader_select_from(char *binario) {
    REGISTRO r;
    FILE *arquivo = fopen(binario, "rb");
    // Verificando se o arquivo está consistente
     if(!consistente(arquivo))
        return false;

    CABECALHO* cabecalho = cabecalho_from_arquivo(arquivo);

    // Salvando o total de registros para iterar sobre o arquivo
    int total=cabecalho_get_nroRegArq(cabecalho)+cabecalho_get_nroRegRem(cabecalho);
    
    // Lendo e imprimindo todos os registros do arquivo, se houver
    if(cabecalho_get_nroRegArq(cabecalho) == 0) {
        printf("Registro inexistente.\n\n");
    }
    else {
        while(total--) {
            r = ler_registro_binario(arquivo);
            imprime_registro(r);
            libera_registro(r);
        }
    }
    cabecalho_apagar(&cabecalho);
    fclose(arquivo);
    return true;
}


//função que realiza as buscas no arquivo principal e printa
bool busca_no_arqDados(FILE* arquivo,REGISTRO registro_buscado,int *procurado){
    int id = registro_buscado.id;
    int idade = registro_buscado.idade;
    char* nomeJogador =  registro_buscado.nomeJogador;
    char* nacionalidade = registro_buscado.nacionalidade;
    char* nomeClube = registro_buscado.nomeClube;
    bool id_encontrado=false;
    bool ok=false;
    // Salvando o numero de registros e registros removidos
    CABECALHO* cabecalho = cabecalho_from_arquivo(arquivo);
    int total = cabecalho_get_nroRegArq(cabecalho) + cabecalho_get_nroRegRem(cabecalho);
    cabecalho_apagar(&cabecalho);
    for(int j = 0; j < total; j++) {
        REGISTRO r = ler_registro_binario(arquivo);
        if(r.removido == '1'){
            libera_registro(r);
            continue;
        }
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
        if(ok) {
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
    char campo[30];         // Campo que o usuário digitará para a busca
    int procurado[6];       // vetor para controle dos campos o usuário está procurando

    // Abrindo e verificando a própria abertura do arquivo
    FILE *arquivo = fopen(binario, "rb");
    if(!consistente(arquivo))return false;


    // Loop externo que executa a qntd de buscas informadas
    for(int i = 0; i < qntd; i++){
        int params;
        scanf("%d",&params);

        // Resetando todos os campos como não procurados pelo usuário na busca
        for(int j=1;j<6;j++)procurado[j] = 0;

        // Recebendo as condições do usuário para cada parametro ad busca atual
        REGISTRO registro_buscado;
        registro_buscado.tamNomeJog=0;
        registro_buscado.tamNomeClube=0;
        registro_buscado.tamNacionalidade=0;
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
                registro_buscado.tamNomeJog = get_tamanho_string(registro_buscado.nomeJogador);
            }
            if(strcmp(campo,"nacionalidade") == 0) {
                procurado[4]=1;
                registro_buscado.nacionalidade = (char*) malloc (sizeof(char)*100);
                scan_quote_string(registro_buscado.nacionalidade);
                registro_buscado.tamNacionalidade = get_tamanho_string(registro_buscado.nacionalidade);
            }
            if(strcmp(campo,"nomeClube") == 0) {
                procurado[5]=1;
                registro_buscado.nomeClube = (char*) malloc (sizeof(char)*100);
                scan_quote_string(registro_buscado.nomeClube);
                registro_buscado.tamNomeClube = get_tamanho_string(registro_buscado.nomeClube);
            }
        }
        //Buscando e printando
        printf("Busca %d\n\n", i + 1);
        //essa função busca e ja printa os registros encontrados,retornando false caso nenhum seja encontrado
        if(!busca_no_arqDados(arquivo,registro_buscado,procurado))printf("Registro inexistente.\n\n");
        libera_registro(registro_buscado);
    }

    fclose(arquivo);
    return true;
}

bool reader_create_index(char *binario, char *indice) {
    long long offsetReg = 25;  // Guarda o offset do registro atual no binário para salvá-lo no índice
    REGISTRO regDados;         // Variável para guardar os registros do arquivo de dados
    REGISTROI *registrosi;     // Vetor para temporariamente armazenar os registros de índice 
    int total;                 // Guarda o número total de registros no arquivo de dados
    int i = 0;                 // Index para o vetor de registrosi

    // Abrindo e verificando o arquivo binário para leitura
    FILE *arquivo = fopen(binario, "rb");
    if(!consistente(arquivo)) return false;
    CABECALHO *cabecalho = cabecalho_from_arquivo(arquivo);

    // Criando o vetor de registros do índice com o número de registros do arquivo
    registrosi = (REGISTROI *) malloc(sizeof(REGISTROI) * cabecalho_get_nroRegArq(cabecalho));
    if(registrosi == NULL)
        return false;
    total = cabecalho_get_nroRegArq(cabecalho) + cabecalho_get_nroRegRem(cabecalho);

    // Percorrendo o binário, criando os regsitros no vetor de registros do índice e o ordenando ao final
    while(total--) {
        regDados = ler_registro_binario(arquivo);
        if(regDados.removido != '1') {
            registrosi[i] = indice_criar_registro(regDados.id, offsetReg);
            i++;
        }

        // Atualizando o offset para o próximo registro
        offsetReg += (long long) regDados.tamanhoRegistro;
        libera_registro(regDados);
    }
    quickSort(registrosi, 0, cabecalho_get_nroRegArq(cabecalho) - 1);

    // Fechando o arquivo binário e abrindo o arquivo de índices para a escrita
    fclose(arquivo);
    bool res = indice_criar(indice, registrosi, cabecalho_get_nroRegArq(cabecalho));

    // Desalocando a memória e retorno da funcionalidade
    cabecalho_apagar(&cabecalho);
    return res;
}

//função que atualiza a lista de removidos e cabeçalho com base no vetor regs que precisa estar ordenado em ordem crescente de tamanho
//essa ordenação diminuirá a complexidade de atualizar a lista ao final de todas as remoções
void atualiza_lista(FILE* arquivo,long long offset_registro_removido,int tamanhoRegistro){
    long long offset_topo = 1;
    //começar a percorrer a lista a partir do primeiro elemento indicado pelo campo topo no cabeçalho
    fseek(arquivo,offset_topo,SEEK_SET);
    long long prox;
    fread(&prox,sizeof(long long),1,arquivo);
    long long offset_anterior = offset_topo;
    while(1){
        if(prox==-1){ //se estiver no final da lista
            //coloca o elemento atual para apontar para o registro que quero inserir na lista de removidos
            fseek(arquivo,-8,SEEK_CUR);
            fwrite(&offset_registro_removido,sizeof(long long),1,arquivo);
            //coloca o proximo da lista como -1,pois sera inserida no final
            fseek(arquivo,offset_registro_removido+5,SEEK_SET);
            prox=-1;
            fwrite(&prox,sizeof(long long),1,arquivo);
            break;
        }
        else{
            //pula para o proximo elemento
            fseek(arquivo,prox+1,SEEK_SET);
            long long atual=prox; //agora o ponteiro esta em prox
            int tamanho_registro_atual=-1;
            fread(&tamanho_registro_atual,sizeof(int),1,arquivo);
            //se o proximo elemento tiver tamanho maior do que o que eu quero inserir
            if(tamanho_registro_atual>=tamanhoRegistro){
                //coloca o elemento removido para apontar para o registro atual
                fseek(arquivo,offset_registro_removido+5,SEEK_SET);
                fwrite(&atual,sizeof(long long),1,arquivo);
                //coloca o elemento anterior para apontar para o registro removido
                fseek(arquivo,offset_anterior,SEEK_SET);
                fwrite(&offset_registro_removido,sizeof(long long),1,arquivo);
                break;
            }
            else{
                offset_anterior = atual+5;
                fread(&prox,sizeof(long long),1,arquivo);
            }  
        } 
    }    
}

//busca e remove os arquivos
int busca_para_remover(FILE* arquivo,REGISTRO registro_buscado,int *procurado,VETREGISTROI* vetor_indices){
    int id = registro_buscado.id;
    int idade = registro_buscado.idade;
    char* nomeJogador =  registro_buscado.nomeJogador;
    char* nacionalidade = registro_buscado.nacionalidade;
    char* nomeClube = registro_buscado.nomeClube;
    // Salvando o numero de registros do arquivo na variavel n_registros
    CABECALHO *cabecalho = cabecalho_from_arquivo(arquivo);
    int total = cabecalho_get_nroRegArq(cabecalho) + cabecalho_get_nroRegRem(cabecalho);
    cabecalho_apagar(&cabecalho);
    // Percorrendo o arquivo para a busca atual
    long long offset_prox_registro = 25;
    long long offset_registro_atual = -1;
    int qntdBuscas = 0;
    for(int j = 0; j < total; j++) {
        REGISTRO r = ler_registro_binario(arquivo);
        if(r.removido == '1'){
            libera_registro(r);
            continue;
        }
        bool ok = true;
        offset_registro_atual = ftell(arquivo)-r.tamanhoRegistro;
        offset_prox_registro=ftell(arquivo);
        for(int k = 1; k <= 5; k++) {
            if(procurado[k] == 0) continue;
            switch(k) {
                case 1: 
                    if(id!=r.id) 
                        ok=false;
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
        if(ok) { //remove o registro

            indice_remover(vetor_indices, r.id); //remove do vetor de indices

            //marcando o registro como removido no arquivo principal
            fseek(arquivo,offset_registro_atual,SEEK_SET);
            char remov='1';
            fwrite(&remov,sizeof(char),1,arquivo);

            //atualiza a lista de registros removidos
            atualiza_lista(arquivo,offset_registro_atual,r.tamanhoRegistro); 
            qntdBuscas += 1;
            
            //voltando o ponteiro para continuar as buscas
            fseek(arquivo,offset_prox_registro,SEEK_SET);
            libera_registro(r);
        }
        else 
            libera_registro(r);
    }
    return qntdBuscas;
}

bool reader_delete_where(char *binario,char *indice,int n){
    // Criando o arquivo de índices
    reader_create_index(binario, indice);

    //traz o arquivo de indices para a ram
    VETREGISTROI *vetor_indices = indice_carregamento(indice, binario);
    if(vetor_indices==NULL)
       return 0;

    FILE* arquivo = fopen(binario,"rb+");
    if(!consistente(arquivo))return 0;  
    set_status_arquivo(arquivo, '0');
    
    int procurado[6]; //variavel e controle para saber quais os campos estao sendo buscados
    char campo[20]; //variavel que vai receber o camp obuscado 
    int busca_total=0;  //variavel para saber o numero de registros deletados

    for(int i = 0; i < n; i++){
        int params;
        scanf("%d",&params);

        // Resetando todos os campos como não procurados pelo usuário na busca
        for(int j=1;j<6;j++)procurado[j] = 0;
        REGISTRO registro_buscado;
        registro_buscado.tamNomeJog=0;
        registro_buscado.tamNomeClube=0;
        registro_buscado.tamNacionalidade=0;
        //recebendo os parametros da busca para deletar
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
                registro_buscado.tamNomeJog = get_tamanho_string(registro_buscado.nomeJogador);
            }
            if(strcmp(campo,"nacionalidade") == 0) {
                procurado[4]=1;
                registro_buscado.nacionalidade = (char*) malloc (sizeof(char)*100);
                scan_quote_string(registro_buscado.nacionalidade);
                registro_buscado.tamNacionalidade = get_tamanho_string(registro_buscado.nacionalidade);
            }
            if(strcmp(campo,"nomeClube") == 0) {
                procurado[5]=1;
                registro_buscado.nomeClube = (char*) malloc (sizeof(char)*100);
                scan_quote_string(registro_buscado.nomeClube);
                registro_buscado.tamNomeClube = get_tamanho_string(registro_buscado.nomeClube);
            }
        }
    
        if(procurado[1]==0){
            //busca no arquivo principal e remove
            int busca_atual = busca_para_remover(arquivo,registro_buscado,procurado,vetor_indices);
            busca_total+=busca_atual;
        }
        else{
            int id=registro_buscado.id;

            //faz a busca binaria no vetor_indices e guarda o resultado em offset_removido
            int pos = indice_buscar(vetor_indices,id); //pos guarda a posição do registro no vetor de indices ou -1 caso nao tenha sido encontrado
            REGISTROI x;
            if(pos!=-1)x = indice_get_registroi_vetor(vetor_indices, pos); //Pega o registro na posição encontrada
            long long offset_removido = x.byteOffset;

            if(pos!=-1){ // remove o registro
                fseek(arquivo,offset_removido,SEEK_SET);
                char removido;
                fread(&removido,sizeof(char),1,arquivo);
                if(removido=='1')continue; // se o registro ja esta removido nao precisa fazer nada
                fseek(arquivo,-1,SEEK_CUR);
                removido='1';
                fwrite(&removido,sizeof(char),1,arquivo);
                int tamanho_reg_rem;
                fread(&tamanho_reg_rem,sizeof(int),1,arquivo);

                //atualiza a lista de registros removidos
                atualiza_lista(arquivo,offset_removido,tamanho_reg_rem);
    
                indice_remover(vetor_indices, id); //remove do vetor_indices

                busca_total+=1;
            }
        }

        //dando free nos campos do registro buscado
        libera_registro(registro_buscado);
    }
    //reescrever o cabecaçalho com o novo n_registros e n_reg_rem
    CABECALHO* cabecalho =  cabecalho_from_arquivo(arquivo);
    cabecalho_set_nroRegArq(cabecalho,cabecalho_get_nroRegArq(cabecalho)-busca_total);
    cabecalho_set_nroRegRem(cabecalho,cabecalho_get_nroRegRem(cabecalho)+busca_total);
    escreve_cabecalho(arquivo,cabecalho);

    //reescrever o arquivo de indices a partir do novo vetor
    indice_reescrita(indice, vetor_indices);
    indice_destruir(&vetor_indices);
    //fechando arquivo
    set_status_arquivo(arquivo, '1');
    fclose(arquivo);

    binarioNaTela(binario);
    binarioNaTela(indice);

    // Desalocando a memória e retornando
    cabecalho_apagar(&cabecalho);
    return true;
}

//insere o registro no arquivo principal e guarda o byte onde foi inserudo
//retorna 1 se o arquivo foi cololocado em espaço de registro logicamente removido
//retorna 0 caso contrario
int inserir_arquivo_principal(FILE* arquivo,REGISTRO* reg,long long *byte){
    int reaproveitados = 0;    // controla a qntd de registros reaproveitados pela estratégia best fit
    long long proxRemovido;    // Topo da lista de registros lógicamente removidos
    long long offset_anterior; // Ponteiro para o registro removido anterior na lista de removidos
    long long proxOffsetLivre; // Próximo offset disponível para inserção ao fim do arquivo
    int tamanho;               // Guarda o tamanho disponível em registros removidos
    int guardar_tamanho;       // Auxiliar para guardar o tamanho original de um registro inserido com best fit
    char lixo = '$';

    // Lendo, do binário, topo e o próximo byteoffest disponível
    fseek(arquivo, 1, SEEK_SET);
    fread(&proxRemovido, sizeof(long long), 1, arquivo);
    fread(&proxOffsetLivre, sizeof(long long), 1, arquivo);
    offset_anterior = 1;

    // Percorrendo a lista de registros removidos para tentar realizar a estratégia
    // best fit
    while(proxRemovido != -1){
        // obtendo o tamanho do removido atual e atualizando o ponteiro para o
        // próximo
        fseek(arquivo, proxRemovido+1, SEEK_SET);
        fread(&tamanho, sizeof(int), 1, arquivo);
        fread(&proxRemovido, sizeof(long long), 1, arquivo);

        // se o tamanho couber o arquivo a ser inserido, então insira e retorne
        if(tamanho >= reg->tamanhoRegistro){
            reaproveitados = 1;

            fseek(arquivo,-13,SEEK_CUR);
            guardar_tamanho = reg->tamanhoRegistro;
            reg->tamanhoRegistro = tamanho;
            *byte = ftell(arquivo);
            escreve_registro(arquivo,reg);

            for(int i = 0; i < tamanho - guardar_tamanho; i++) {
                fwrite(&lixo, sizeof(char), 1, arquivo);
            }
            //coloca o anterior para apontar para o proximo
            fseek(arquivo,offset_anterior,SEEK_SET);
            fwrite(&proxRemovido,sizeof(long long),1,arquivo);
            return reaproveitados;
        }

        //caso contrario continue o loop e guarde o offset;
        else{
            offset_anterior = ftell(arquivo) - 8;
        }
    }

    // Caso não haja nenhum removido reaproveitados,; insira no fim
    fseek(arquivo,0,SEEK_END);
    *byte = ftell(arquivo);
    escreve_registro(arquivo,reg);

    // Atualizando o próximo byteoffset
    proxOffsetLivre += reg->tamanhoRegistro;
    fseek(arquivo, 9, SEEK_SET);
    fwrite(&proxOffsetLivre, sizeof(long long), 1, arquivo);
    return reaproveitados;
}

// Função que realiza novas inserções no arquivo principal, retornando o que deve
// ser inserido em um arquivo de índices (tando índice simples como árvore B)
bool reader_insert(char *binario, int n, VETREGISTROI *vetor_indices, ARVORE_B* arvore){
    FILE* arquivo;               // Ponteiro para o arquivo
    int inseridos = 0;           // Retorna os registros que realmente foram inseridos
    int reaproveitados = 0;      // Controla a qntd de registros reaproveitados pela estratégia best fit
    REGISTRO r;                  // Guarda os novos registros a serem inseridos
    char strAux[10];             // Auxiliar para tratar registros fixos com valor NULO
    long long byte;              // Guarda o byteoffest de cada registro no arquivo de dados para criar o registroi
    int regInvalido = 0;         // Guarda o número de registros invalidos que não foram inseridos
    long long jaInserido;        // Recebe os resultados das buscas para verificar se o registro já foi inserido
    
    // Abrindo e verificando a consistência do arquivo binário principal
    arquivo = fopen(binario,"rb+");
    if(!consistente(arquivo))return 0;

    // Setando o status para inconsistente como medida preventiva
    set_status_arquivo(arquivo, '0');

    // Coletando os dados e inserindo nos binários n vezes
    for(int i=0;i<n;i++){
        // Lendo os campos fixos
        scan_quote_string(strAux);
        if(strcmp(strAux, "") == 0) {
            regInvalido++;
            continue; // id nulo torna o registro inválido
        }
        else
            r.id = strToInt(strAux, get_tamanho_string(strAux));

        scan_quote_string(strAux);
        if(strcmp(strAux, "") == 0)
            r.idade = -1;
        else
            r.idade = strToInt(strAux, 2);

        // Lendo os campos de tamanho variável
        r.nomeJogador = (char*) malloc (sizeof(char)*100);
        r.nacionalidade = (char*) malloc(sizeof(char)*100);
        r.nomeClube = (char*) malloc(sizeof(char)*100);
        r.removido = '0';
        r.prox = -1;
        scan_quote_string(r.nomeJogador);
        scan_quote_string(r.nacionalidade);
        scan_quote_string(r.nomeClube);
        r.tamNomeJog = get_tamanho_string(r.nomeJogador);
        if(r.tamNomeJog == 0)
            free(r.nomeJogador);

        r.tamNacionalidade = get_tamanho_string(r.nacionalidade);
        if(r.tamNacionalidade == 0)
            free(r.nacionalidade);
        
        r.tamNomeClube = get_tamanho_string(r.nomeClube);
        if(r.tamNomeClube == 0)
            free(r.nomeClube);

        r.tamanhoRegistro = 33 + r.tamNomeJog + r.tamNacionalidade + r.tamNomeClube;

        // Verificando se o registro já existe no arquivo de índices correpondente
        if(arvore != NULL) {
            jaInserido = (long long) arvore_buscar(arvore, r.id);
        }
        else
            jaInserido = (long long) indice_buscar(vetor_indices, r.id);

        if(jaInserido == -1) {
            // inserindo no arquivo principal e guardando o byte que foi inserido para o vetor de índices
            reaproveitados+=inserir_arquivo_principal(arquivo, &r, &byte);
            indice_inserir(vetor_indices, indice_criar_registro(r.id, byte));
            inseridos++;
            libera_registro(r);
        }
    }

    //atualizando  o n_registros e o n_registros_removidos do arquivo
    CABECALHO *cabecalho = cabecalho_from_arquivo(arquivo);
    cabecalho_set_nroRegArq(cabecalho,cabecalho_get_nroRegArq(cabecalho)+inseridos - regInvalido);
    cabecalho_set_nroRegRem(cabecalho,cabecalho_get_nroRegRem(cabecalho)-reaproveitados);
    escreve_cabecalho(arquivo,cabecalho);
    cabecalho_apagar(&cabecalho);

    // Voltando o status para 1
    set_status_arquivo(arquivo, '1');
    fclose(arquivo);

    // Desalocando registros e retornando
    cabecalho_apagar(&cabecalho);
    return inseridos;
}

bool reader_insert_into(char *binario, char *indice, int n) {
    VETREGISTROI *vetor_indices; // Vetor para carregar o arquivo de índices
    int res;

    // Criando o arquivo de índices
    reader_create_index(binario, indice);
    vetor_indices = indice_carregamento(indice, binario);
    if(vetor_indices == NULL)
        return false;

    // Chamando a função de inserção
    res = reader_insert(binario, n, vetor_indices, NULL);
    if(res != 0) {
        indice_destruir(&vetor_indices);
        return res;
    }

    // Reescrevendo o arquivo de índices
    indice_reescrita(indice, vetor_indices);
    indice_destruir(&vetor_indices);

    // Chamando binario na tela para os arquivos
    binarioNaTela(binario);
    binarioNaTela(indice);
    return true;
}

// Função auxiliar que, dado um arquivo aberto e consistente e um offset, retorna 
// o REGISTRO referente a ele
REGISTRO get_registro_offset(FILE *binario, long long offset) {
    REGISTRO r;

    fseek(binario, offset, SEEK_SET);
    r = ler_registro_binario(binario);
    return r;
}

bool reader_select_from_id(char *binario, char *indice, int n) {
    REGISTRO r;
    FILE *arqBinario;
    ARVORE_B *arvore;
    long long offSetBuscado;
    int chaveBuscada;

    // Abrindo ambos arquivos para leitura
    arqBinario = fopen(binario, "rb");
    if(!consistente(arqBinario)) {
        return false;
    }
    
    // Instanciando o cabeçalho do arquivo de índices na memória
    arvore = arvore_carregar_cabecalho(indice);
    if(arvore == NULL)
        return false;

    // Loop para as buscas
    for(int i = 0; i < n; i++) {
        scanf(" id %d", &chaveBuscada);
        printf("Busca %d\n\n", i + 1);

        offSetBuscado = arvore_buscar(arvore, chaveBuscada);
        if(offSetBuscado != -1) {
            r = get_registro_offset(arqBinario, offSetBuscado);
            imprime_registro(r);
        }
        else
            printf("Registro inexistente\n\n");
    }
    return true;
}

bool reader_insert_into_bTree(char *binario, char *indice, int n) {
    VETREGISTROI *registrosi;    // Vetor dos registros a serem adcionados na árvore b
    REGISTROI regi;              // Auxiliar que recebe um regsitroi do vetor
    ARVORE_B *arvore;
    int res;
    
    // Instanciando o cabeçalho da árvore para as inserções
    arvore = arvore_carregar_cabecalho(indice);
    if(arvore == NULL)
        return false;

    // Instanciando um novo VETREGISTROI com o número de elementos a serem inseridos
    registrosi = indice_criar_vetor(n);
    if(registrosi == NULL)
        return false;

    // Chamando a função de inserção
    res = reader_insert(binario, n, registrosi, arvore);

    // Reescrevendo o arquivo de índices
    for(int i = 0; i < res; i++) {
        regi = indice_get_registroi_vetor(registrosi, i);
        res = arvore_inserir(arvore, regi.id, regi.byteOffset);
    }
    indice_destruir(&registrosi);
    arvore_destruir(&arvore);

    // Chamando binario na tela para os arquivos
    binarioNaTela(binario);
    binarioNaTela(indice);
    return true;
}

bool create_index_arvore_B(char *binario,char* indice ){
    //FILE* arqq = fopen(indice,"wb");
    //fclose(arqq);
    long long offsetReg = 25;  // Guarda o offset do registro atual no binário para salvá-lo no índice
    REGISTRO regDados;         // Variável para guardar os registros do arquivo de dados
    int total;                 // Guarda o número total de registros no arquivo de dados

    // Abrindo e verificando o arquivo binário para leitura
    FILE *arquivo = fopen(binario, "rb");
    if(!consistente(arquivo)) return false;
    CABECALHO *cabecalho = cabecalho_from_arquivo(arquivo);

    total = cabecalho_get_nroRegArq(cabecalho) + cabecalho_get_nroRegRem(cabecalho);
    
    ARVORE_B* arvore = arvore_criar(indice);

    bool res=true;
    // Percorrendo o binário, criando os regsitros no vetor de registros do índice e o ordenando ao final
    while(total--) {
        regDados = ler_registro_binario(arquivo);
        if(regDados.removido != '1') {
            printf("Inserção atual: %d\n",regDados.id);
            arvore_inserir(arvore,regDados.id,offsetReg);
            //printf("%d\n",regDados.id);
        }

        // Atualizando o offset para o próximo registro
        offsetReg += (long long) regDados.tamanhoRegistro;
        libera_registro(regDados);
    }

    // Fechando o arquivo binário 
    fclose(arquivo);

    // Desalocando a memória e retorno da funcionalidade
    cabecalho_apagar(&cabecalho);
    binarioNaTela(indice);
    arvore_destruir(&arvore);
    return true;
}

bool funcionalidade_9(char* binario,char* indice, int n){
    // Criando o arquivo de índices
    ARVORE_B* arvore = arvore_carregar_cabecalho(indice);
    
    // Abrindo e verificando o arquivo binário
    FILE* arquivo = fopen(binario,"rb");
    if(!consistente(arquivo))return false;  
    set_status_arquivo(arquivo, '0');
    
    // Verificando o arquivo de índices para o caso dele estar inconsistente
    FILE *arqIndice = fopen(indice, "rb");
    if(!consistente(arqIndice))
        return false;
    fclose(arqIndice);

    int procurado[6]; //variavel e controle para saber quais os campos estao sendo buscados
    char campo[20]; //variavel que vai receber o camp obuscado 
    int busca_total=0;  //variavel para saber o numero de registros deletados

    for(int i = 0; i < n; i++){
        printf("Busca %d\n\n", i + 1);

        int params;
        scanf("%d",&params);

        // Resetando todos os campos como não procurados pelo usuário na busca
        for(int j=1;j<6;j++)procurado[j] = 0;
        REGISTRO registro_buscado;
        registro_buscado.tamNomeJog=0;
        registro_buscado.tamNomeClube=0;
        registro_buscado.tamNacionalidade=0;
        //recebendo os parametros da busca 
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
                registro_buscado.tamNomeJog = get_tamanho_string(registro_buscado.nomeJogador);
            }
            if(strcmp(campo,"nacionalidade") == 0) {
                procurado[4]=1;
                registro_buscado.nacionalidade = (char*) malloc (sizeof(char)*100);
                scan_quote_string(registro_buscado.nacionalidade);
                registro_buscado.tamNacionalidade = get_tamanho_string(registro_buscado.nacionalidade);
            }
            if(strcmp(campo,"nomeClube") == 0) {
                procurado[5]=1;
                registro_buscado.nomeClube = (char*) malloc (sizeof(char)*100);
                scan_quote_string(registro_buscado.nomeClube);
                registro_buscado.tamNomeClube = get_tamanho_string(registro_buscado.nomeClube);
            }
        }
    
        if(procurado[1]==0){
            busca_no_arqDados(arquivo,registro_buscado,procurado);
        }
        else{
            int id=registro_buscado.id;
            long long offset_encontrado = arvore_buscar(arvore,id);

            if(offset_encontrado!=-1){ // printa o registro
               fseek(arquivo,offset_encontrado,SEEK_SET);
               REGISTRO reg = ler_registro_binario(arquivo);
               imprime_registro(reg);
               libera_registro(reg);
            }
            else{
                printf("Registro inexistente.\n");
            }
        }

        //dando free nos campos do registro buscado
        libera_registro(registro_buscado);
    }
  
    //fechando arquivo
    set_status_arquivo(arquivo, '1');
    fclose(arquivo);

    return true;
}

