#include "arvore_b.h"
#include "util.h"
typedef long long ll;

#define TAMREGISTRO 60

typedef struct elemento{
    int chave;
    long long offset;
}elem;

typedef struct node{
    int altura_no;
    int n_chaves;
    int RRN_filhos[4]; //vetor com os RRNs de todos os filhos desse no
    elem elementos[3]; //vetor com todos os elementos desse no
}NO;

struct arvore_b{
    char status;
    int RRN_raiz;
    int proxRRN;
    int nroChaves;
    char indices[40]; // Armazena o nome do arquivo de índices
};


// Função que efetivamente cria o arquivo de índices, escrevendo seu cabeçalho
void indices_escreve_cabecalho(ARVORE_B *arvore, FILE *indices) {
    // Setando as variáveis do cabeçalho inicial
    arvore->status = '1';
    arvore->RRN_raiz = -1;
    arvore->proxRRN = 0;
    arvore->nroChaves = 0;
    const char *lixo = "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"; 

    // Escrevendo o cabeçalho
    fwrite(&arvore->status, sizeof(char), 1, indices);
    fwrite(&arvore->RRN_raiz, sizeof(int), 1, indices);
    fwrite(&arvore->proxRRN, sizeof(int), 1, indices);
    fwrite(&arvore->nroChaves, sizeof(int), 1, indices);
    fwrite(lixo, 47, 1, indices);
    return;
}

// Função auxiliar que, para cada inserção, ajusta o cabeçalho de forma a economizar a quantidade de seeks
void ajustaCabecalho(ARVORE_B *arvore, FILE *indices) {
   
    fseek(indices,1,SEEK_SET);
    fwrite(&arvore->RRN_raiz,sizeof(int),1,indices);
    fwrite(&arvore->proxRRN,sizeof(int),1,indices);
    fwrite(&arvore->nroChaves,sizeof(int),1,indices);
    
}

// Função auxiliar que escreve um novo registro (nó) no índice da árvore b
bool indices_escreve_registro(FILE *indices, NO no, int RRN_NO) {
    int offset;     // Recebe o offset exato do nó a partir de RRN_NO

    // offset = tamCabecalho + rrn * tamRegistro (porém ambos tem 60 bytes)
    offset = (RRN_NO + 1) * TAMREGISTRO;

    // Escrevendo o nó
    fseek(indices, offset, SEEK_SET);
    fwrite(&no.altura_no, sizeof(int), 1, indices);
    fwrite(&no.n_chaves, sizeof(int), 1, indices);
    
    for(int i = 0; i < 3; i++) {
        fwrite(&no.elementos[i].chave, sizeof(int), 1, indices);
        fwrite(&no.elementos[i].offset, sizeof(long long), 1, indices);
    }

    for(int i = 0; i < 4; i++) {
        fwrite(&no.RRN_filhos[i], sizeof(int), 1, indices);
    }

    return true;
}

ARVORE_B* arvore_criar(char* indices){
    FILE *arquivo;
    ARVORE_B* arvore = (ARVORE_B*)malloc(sizeof(ARVORE_B));

    // Criando o arquivo de índices
    arquivo = fopen(indices, "wb");
    if(arquivo == NULL)
        return NULL;
    indices_escreve_cabecalho(arvore, arquivo);

    strcpy(arvore->indices,indices);
    fclose(arquivo);
    return arvore;
}

// Função auxiliar que, instancia o cabeçalho de um arquivo de índices já criado
ARVORE_B *arvore_carregar_cabecalho(char *indices) {
    ARVORE_B *arvore;
    FILE *arquivo;

    // Abrino e verificando o arquivo
    arquivo = fopen(indices, "rb");
    if(!consistente(arquivo))
        return NULL;

    if(ftell(arquivo) != 0)
        fseek(arquivo, 0, SEEK_SET);

    // Alocando memória para a árvore
    arvore = (ARVORE_B *) malloc(sizeof(ARVORE_B));
    if(arvore == NULL) {
        fclose(arquivo);
        return NULL;
    }

    fread(&arvore->status, sizeof(char), 1, arquivo);
    fread(&arvore->RRN_raiz, sizeof(int), 1, arquivo);
    fread(&arvore->proxRRN, sizeof(int), 1, arquivo);
    fread(&arvore->nroChaves, sizeof(int), 1, arquivo);
    strcpy(arvore->indices, indices);

    fclose(arquivo);
    return arvore;
}

void arvore_destruir(ARVORE_B** arvore){
    free(*arvore);
    *arvore = NULL;
}

// Função auxiliar que, dado um RRN, busca o nó correspondente no arquivo de índices
// e o instancía no memória.
NO criar_no(FILE *indices, int RRN_NO) {
    NO novoNo;
    int offset;     // Recebe o offset exato do nó a partir de RRN_NO

    offset = (RRN_NO + 1) * TAMREGISTRO;

    //Saltando para a posição do nó
    fseek(indices, offset, SEEK_SET);
    fread(&novoNo.altura_no, sizeof(int), 1, indices);
    fread(&novoNo.n_chaves, sizeof(int), 1, indices);
    
    // Lendo todos as chaves (elementos) do nó autal
    for(int i = 0; i < 3; i++) {
        fread(&novoNo.elementos[i].chave, sizeof(int), 1, indices);
        fread(&novoNo.elementos[i].offset, sizeof(ll), 1, indices);
        //fread(&novoNo.elementos[i], sizeof(elem), 1, indices);
    }

    // Lendo todos os rrns para os filhos do nó atual
    for(int i = 0; i < 4; i++) {
        fread(&novoNo.RRN_filhos[i], sizeof(int), 1, indices);
    }

    return novoNo;
}

ll arvore_buscar_recursivo(int RRN_NO, int chave,FILE* indices){
    if(RRN_NO==-1)return -1;

    // Trazendo o nó atual para a RAM
    NO no = criar_no(indices,RRN_NO);
    elem* elementos = no.elementos;
    int* RRN_filhos = no.RRN_filhos;

    // Buscando nas chaves do nó atual
    for(int i=0;i<no.n_chaves;i++){
        if(elementos[i].chave==chave){
            ll res = elementos[i].offset;
            return res;
        }
        // Caso em que indentifica-se que a chave buscada está em um nível abaixo
        else if(elementos[i].chave>chave){
            int RRN_filho = RRN_filhos[i];
            return arvore_buscar_recursivo(RRN_filho,chave,indices);
        }
    }

    // Caso a chave buscada seja maior que todas no nó atual
    int RRN_filho = RRN_filhos[no.n_chaves];
    return arvore_buscar_recursivo(RRN_filho,chave,indices);
}

ll arvore_buscar(ARVORE_B* arvore,int chave){
    ll valorBuscado;

    FILE* arquivo = fopen(arvore->indices,"rb");
    if(!consistente(arquivo))
        return -1;

    valorBuscado = arvore_buscar_recursivo(arvore->RRN_raiz,chave,arquivo);
    fclose(arquivo);

    return valorBuscado; 
}

// Função auxiliar que retorna o campo proxRRN do cabeçalho da árvore b em memória
int get_prox_RRN(ARVORE_B *arvore) {
    if(arvore != NULL)
        return arvore->proxRRN;
    return -1;
}

// Função auxilar que autera o campo proxRRN do cabeçalho da árvore b em memória
void set_prox_RRN(ARVORE_B *arvore, int novoRRN) {
    if(arvore != NULL)
        arvore->proxRRN = novoRRN;
    return;
}

// Função auxiliar que muda o status do cabeçalho, para quando o arquivo é aberto para escrita
void set_status_cabecalho(ARVORE_B *arvore, FILE *indices, char newStatus) {
    arvore->status = newStatus;
    
    if(ftell(indices) != 0)
        fseek(indices, 0, SEEK_SET);
    fwrite(&newStatus, sizeof(char), 1, indices);
}

elem inserir_elemento_em_no(NO no, elem new_elem, int RRN_NO, ARVORE_B* arvore,FILE* indices){ //insere o elemento e retorna o elemento que vai subir do split, caso haja
        elem empty = {-1, -1} ;      
        elem* elementos = no.elementos;
        int* RRN_filhos = no.RRN_filhos;
        int chave = new_elem.chave;
        bool ok=true;
        if(no.n_chaves < 3){ //insere nesse NO o novo elemento, sem split
            //printf("Inserção sem split\n");
            no.n_chaves++;
            //rearranjando os elementos
            for(int j = no.n_chaves - 2; j>=0; j--){
                if(elementos[j].chave > chave){
                    elementos[j + 1] = elementos[j]; 
                }
                else{
                    elementos[j+1] = new_elem;
                    ok=false;
                    break;
                }
            }
            if(ok)elementos[0] = new_elem; // If new_elem is the smallest

            //rearranjando os filhos
            for(int j = no.n_chaves - 1; j>=0; j--){
                if(elementos[j].chave != chave){
                    RRN_filhos[j+1] = RRN_filhos[j];
                }
                else{
                    RRN_filhos[j+1] = get_prox_RRN(arvore) - 1;
                    break;
                }
            }

            if(no.altura_no==0){
                for(int i=0;i<4;i++){
                    RRN_filhos[i]=-1;
                }
            }
            // for(int i=0;i<4;i++)printf("%d ",RRN_filhos[i]);
            // for(int i=0;i<3;i++)printf("%d %lld\n",elementos[i].chave,elementos[i].offset);

            indices_escreve_registro(indices, no, RRN_NO);
            return empty;
            
        }
        else{ //insere com split    --> sempre atualizar o proxRRN
            //printf("Inserção com split\n");

            int prox_RRN = get_prox_RRN(arvore);
            set_prox_RRN(arvore, prox_RRN+1);

            //Criano o novo no
            NO new_no;
            new_no.n_chaves = 2; //será sempre 2 o numero de chaves do novo nó criado com split
            no.n_chaves = 1 ;    //será sempre 1 o numero de chaves do no antigo após o split
            new_no.altura_no = no.altura_no;

            //fazendo vetores temporarios para auxiliar no split
            elem temporario[4];
            int filhos_temporario[5];
            int aux=0;
            for(int j=0;j<4;j++){
                if( j == 3 && aux == 0){
                    temporario[j] = new_elem;
                    filhos_temporario[j] = RRN_filhos[j];
                    filhos_temporario[j+1] = prox_RRN - 1;
                }
                else if( (elementos[j].chave>chave) && aux==0 ){
                    temporario[j] = new_elem;
                    filhos_temporario[j] = RRN_filhos[j];
                    filhos_temporario[j+1] = prox_RRN - 1;
                    aux=1;
                }
                else {
                    temporario[j] = elementos[j-aux];
                    filhos_temporario[j+aux] = RRN_filhos[j];
                    
                }
            }

            // vetores temporarios feitos

            elementos[0] = temporario[0];
            elementos[1] = empty;
            elementos[2] = empty;

            new_no.elementos[0] = temporario[2];
            new_no.elementos[1] = temporario[3];
            new_no.elementos[2] = empty;
        
            RRN_filhos[0] = filhos_temporario[0];
            RRN_filhos[1] = filhos_temporario[1];
            RRN_filhos[2] = -1;
            RRN_filhos[3] = -1;

            new_no.RRN_filhos[0] = filhos_temporario[2];
            new_no.RRN_filhos[1] = filhos_temporario[3];
            new_no.RRN_filhos[2] = filhos_temporario[4];
            new_no.RRN_filhos[3] = -1;

            if(no.altura_no==0){
                for(int i=0;i<4;i++){
                    RRN_filhos[i]=-1;
                    new_no.RRN_filhos[i]=-1;
                }
            }
            
            //novo no feito
            indices_escreve_registro(indices,new_no,prox_RRN);
            indices_escreve_registro(indices,no,RRN_NO);
            

            if(RRN_NO == arvore->RRN_raiz){//terá que criar um novo nó para ser a nova raiz a árvore
                int RRN_raiz = get_prox_RRN(arvore);
                set_prox_RRN(arvore,RRN_raiz+1);
                NO nova_raiz;
                nova_raiz.altura_no=no.altura_no+1;
                nova_raiz.n_chaves = 1;

                nova_raiz.elementos[0] = temporario[1];
                nova_raiz.elementos[1] = empty;
                nova_raiz.elementos[2] = empty;

                nova_raiz.RRN_filhos[0] = RRN_NO;
                nova_raiz.RRN_filhos[1] = prox_RRN;
                nova_raiz.RRN_filhos[2] = -1;
                nova_raiz.RRN_filhos[3] = -1;
                
                arvore->RRN_raiz = RRN_raiz;
                indices_escreve_registro(indices,nova_raiz,RRN_raiz);
                return empty;
            }
            else return temporario[1];
        }
}

elem arvore_inserir_recursivo(int RRN_NO,int chave,ll offset, FILE* indices,ARVORE_B* arvore){
    elem empty = {-1,-1};
    if(arvore->nroChaves==1){ //arvore vazia, inserção do primeiro elemento
        //printf("Insercao na raiz\n");
        set_prox_RRN(arvore,1);
        arvore->RRN_raiz = 0;
        elem new_elem;
        new_elem.chave = chave;
        new_elem.offset = offset;
        NO new_no;
        new_no.altura_no = 0;
        new_no.n_chaves = 1;
        for(int i=0;i<4;i++)new_no.RRN_filhos[i]=-1;
        new_no.elementos[0]=new_elem;
        new_no.elementos[1] = empty;
        new_no.elementos[2] = empty;
        indices_escreve_registro(indices,new_no,0);
        return empty;
    }


    NO no = criar_no(indices,RRN_NO);
    imprimeNO(no);

    elem* elementos = no.elementos;
    int* RRN_filhos = no.RRN_filhos;

    for(int i=0;i<no.n_chaves;i++){
        if(elementos[i].chave==chave){
            arvore->nroChaves--;
            return empty;
        }
        if(elementos[i].chave>chave || i == no.n_chaves - 1){
            int RRN_filho = RRN_filhos[i];
            //condicao para o elemento ser inserido no filho mais a direita
            if(i == no.n_chaves - 1 && elementos[i].chave < chave){
                RRN_filho = RRN_filhos[i+1];
            }

            if(RRN_filho==-1){ //A inserção vai ocorrer agora, pois chegamos em uma folha
                //printf("Inserindo no no %d",RRN_NO);
                //Criando o elemento que vai ser inserido
                elem new_elem;
                new_elem.chave = chave;
                new_elem.offset = offset;                
               
                return inserir_elemento_em_no(no, new_elem, RRN_NO, arvore,indices);

            }
            else { //a inserção ocorrerá no filho
                //printf("Passando pelo no %d e indo para o filho %d\n",RRN_NO,RRN_filho);
                elem elemento_a_ser_inserido = arvore_inserir_recursivo(RRN_filho,chave,offset, indices, arvore);
                if(elemento_a_ser_inserido.chave == -1) return elemento_a_ser_inserido;

                //inserir o elemento_a_ser_inserido na posicao e retornar o elemento que subirá do split,caso haja
                return inserir_elemento_em_no(no, elemento_a_ser_inserido, RRN_NO, arvore,indices);
            }
        }
    }
}

bool arvore_inserir(ARVORE_B* arvore,int chave,ll offset){
    FILE* arquivo = fopen(arvore->indices,"rb+");
    if(!consistente(arquivo) || arvore == NULL)
        return false;

    set_status_cabecalho(arvore, arquivo, '0');
    arvore->nroChaves++;
    arvore_inserir_recursivo(arvore->RRN_raiz,chave, offset,arquivo, arvore);
    ajustaCabecalho(arvore,arquivo);

    set_status_cabecalho(arvore, arquivo, '1');
    fclose(arquivo);
    return true;
}


// DEBUGS
void imprimeNO(NO no) {
    printf("altura: %d\n", no.altura_no);
    printf("n_chaves: %d\n", no.n_chaves);
    
    printf("RRN_FILHOS: ");
    for(int i = 0; i < 4; i++) {
        printf("%d ", no.RRN_filhos[i]);
    }
    printf("\n\n");

    printf("ELEMENTOS: ");
    for(int i = 0; i < 3; i++) {
        printf("%d ", no.elementos[i].chave);
    }
    printf("\n\n");
}

void imprime_recursivo(int RRN,FILE *indices){
    if(RRN==-1)return;
    NO atual = criar_no(indices,RRN);
    imprimeNO(atual);
  
}

void imprime_arvore(ARVORE_B* arvore,FILE* indices){
    imprime_recursivo(arvore->RRN_raiz,indices);
}

void imprimeCabecalho(ARVORE_B *arv) {
    if(arv != NULL) {
        printf("status: %c\n", arv->status);
        printf("rrnRaiz: %d\n", arv->RRN_raiz);
        printf("proxRRn: %d\n", arv->proxRRN);
        printf("nroChaves: %d\n", arv->nroChaves);
        printf("indices: %s\n\n", arv->indices);
    }
}
