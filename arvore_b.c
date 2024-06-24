#include "arvore_b.h"
#include "util.h"
typedef long long ll;

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
    int RRN_raiz;
    char indices[40];
};


ARVORE_B* arvore_criar(char* indices){
    ARVORE_B* arvore = (ARVORE_B*)malloc(sizeof(ARVORE_B));

    NO raiz;
    raiz.altura_no = 0;
    raiz.n_chaves = 0;
    for(int i=0;i<4;i++){
        raiz.RRN_filhos[i]=-1;
    }
     for (int i = 0; i < 3; i++) {
        raiz.elementos[i].chave = -1;
        raiz.elementos[i].offset = -1;
    }
    indices_escreve_registro(indices,raiz,0);

    arvore->RRN_raiz = 0;
    strcpy(arvore->indices,indices);
    return arvore;
}

void arvore_destruir(ARVORE_B** arvore){
    free(*arvore);
    *arvore = NULL;
}

ll arvore_buscar_recursivo(int RRN_NO, int chave,FILE* indices){
    if(RRN_NO==-1)return -1;
    NO no = criar_no(indices,RRN_NO);
    elem* elementos = no.elementos;
    int* RRN_filhos = no.RRN_filhos;
    for(int i=0;i<no.n_chaves;i++){
        if(elementos[i].chave==chave){
            ll res = elementos[i].offset;
            return res;
        }
        else if(elementos[i].chave>chave){
            ll RRN_filho = RRN_filhos[i];
            return arvore_buscar_recursivo(RRN_filho,chave,indices);
        }
    }
    ll RRN_filho = RRN_filhos[no.n_chaves];
    return arvore_buscar_recursivo(RRN_filho,chave,indices);
}

ll arvore_buscar(ARVORE_B* arvore,int chave){
    FILE* arquivo = fopen(arvore->indices,"rb");
    ll res = arvore_buscar_recursivo(arvore->RRN_raiz,chave,arquivo);
    fclose(arquivo);
    return res;
}

elem inserir_elemento_em_no(NO no, elem new_elem,int RRN_NO, ARVORE_B* arvore,FILE* indices){ //insere o elemento e retorna o elemento que vai subir do split, caso haja
        elem* elementos = no.elementos;
        int* RRN_filhos = no.RRN_filhos;
        int chave = new_elem.chave;
        if(no.n_chaves < 3){ //insere nesse NO o novo elemento, sem split
            no.n_chaves++;
            for(int j=no.n_chaves - 2;j>=0;j--){
                if(elementos[j].chave>chave){
                    elementos[j+1] = elementos[j];
                }
                else{
                    elementos[j+1] = new_elem;
                    indices_escreve_registro(indices,no,RRN_NO);
                    elem empty_elem = { -1, -1 }; // Indicating no element to return
                    return empty_elem;
                }
            }
            elementos[0] = new_elem; // If new_elem is the smallest
            indices_escreve_registro(indices, no, RRN_NO);
            elem empty_elem = { -1, -1 }; // Indicating no element to return
            return empty_elem;
        }
        else{ //insere com split


            int prox_RRN = get_prox_RRN(indices);
            set_prox_RRN(indices,prox_RRN+1);

            //Criano o novo no
            NO new_no;
            new_no.n_chaves = 2; //será sempre 2 o numero de chaves do novo nó criado com split
            no.n_chaves = 1;//será sempre 1 o numero de chaves do no antigo após o split
            new_no.altura_no = no.altura_no;

            //fazendo vetores temporarios para auxiliar no split
            elem temporario[4];
            int filhos_temporario[5];
            int aux=0;
            for(int j=0;j<4;j++){
                if( j == 3 && aux == 0){
                    temporario[j] = new_elem;
                    filhos_temporario[j] = RRN_filhos[j];
                    filhos_temporario[j+1] = prox_RRN;
                }
                else if( (elementos[j].chave>chave) && aux==0 ){
                    temporario[j] = new_elem;
                    filhos_temporario[j] = RRN_filhos[j];
                    filhos_temporario[j+1] = prox_RRN;
                    aux=1;
                }
                else {
                    temporario[j] = elementos[j-aux];
                    filhos_temporario[j+aux] = RRN_filhos[j];
                    
                }
            }

            // vetores temporarios feitos

            elementos[0] = temporario[0];
            new_no.elementos[0] = temporario[2];
            new_no.elementos[1] = temporario[3];
        
            RRN_filhos[0] = filhos_temporario[0];
            RRN_filhos[1] = filhos_temporario[1];

            new_no.RRN_filhos[0] = filhos_temporario[2];
            new_no.RRN_filhos[1] = filhos_temporario[3];
            new_no.RRN_filhos[2] = filhos_temporario[4];
            
            //novo no feito
            indices_escreve_registro(indices,new_no,prox_RRN);
            indices_escreve_registro(indices,no,RRN_NO);
            if(no.altura_no == 0){//terá que criar um novo nó para ser a nova raiz da árvore
                int RRN_raiz = get_prox_RRN(indices);
                set_prox_RRN(indices,RRN_raiz+1);
                NO nova_raiz;
                nova_raiz.altura_no=no.altura_no+1;
                nova_raiz.n_chaves = 1;
                nova_raiz.elementos[0] = temporario[1];
                nova_raiz.RRN_filhos[0] = RRN_NO;
                nova_raiz.RRN_filhos[1] = prox_RRN;
                indices_escreve_registro(indices,nova_raiz,RRN_raiz);
                set_raiz(indices,RRN_raiz);
                arvore->RRN_raiz = RRN_raiz;
                elem empty_elem = { -1, -1 }; // Indicating no element to return
                return empty_elem;
            }
            else return temporario[1];
        }
}

elem arvore_inserir_recursivo(int RRN_NO,int chave,ll offset, FILE* indices,ARVORE_B* arvore){
    NO no = criar_no(indices,RRN_NO);

    if(no.n_chaves==0){ //arvore vazia, inserção do primeiro elemento
        elem new_elem;
        new_elem.chave = chave;
        new_elem.offset = offset;
        no.elementos[0] = new_elem;
        no.n_chaves++;
        indices_escreve_registro(indices,no,RRN_NO);
        elem empty_elem = { -1, -1 }; // Indicating no element to return
        return empty_elem;
    }

    elem* elementos = no.elementos;
    int* RRN_filhos = no.RRN_filhos;
    for(int i=0;i<no.n_chaves;i++){
        if(elementos[i].chave>chave || i == no.n_chaves - 1){
            ll RRN_filho = RRN_filhos[i];
            //condicao para o elemento ser inserido no filho mais a direita
            if(i == no.n_chaves - 1 && elementos[i].chave < chave){
                RRN_filho = RRN_filhos[i+1];
            }

            if(RRN_filho==-1){ //A inserção vai ocorrer agora, pois chegamos em uma folha

                //Criando o elemento que vai ser inserido
                elem new_elem;
                new_elem.chave = chave;
                new_elem.offset = offset;                
               
                return inserir_elemento_em_no(no, new_elem, RRN_NO, arvore,indices);

            }

            else { //a inserção ocorrerá no filho
                elem elemento_a_ser_inserido = arvore_inserir_recursivo(RRN_filho,chave,offset, indices, arvore);
                if(elemento_a_ser_inserido.chave == -1) return elemento_a_ser_inserido;

                //inserir o elemento_a_ser_inserido na posicao e retornar o elemento que subirá do split,caso haja
                return inserir_elemento_em_no(no, elemento_a_ser_inserido, RRN_NO, arvore,indices);
            }
        }
    }
    elem empty_elem = { -1, -1 }; // Indicating no element to return
    return empty_elem;
}

void arvore_inserir(ARVORE_B* arvore,int chave,ll offset){
    FILE* arquivo = fopen(arvore->indices,"rb+");
    arvore_inserir_recursivo(arvore->RRN_raiz,chave, offset,arquivo, arvore);
    fclose(arquivo);
}
