#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

#define TAM_HASH 1009


// guarda onde a palavra apareceu
typedef struct local{

    char nomeArquivo[260];
    long offset;

    struct local *prox;

}Local;


// guarda a palavra e a lista de locais dela
typedef struct palavra{

    char texto[51];

    Local *locais;

    struct palavra *prox;

}Palavra;


// tabela hash
Palavra *tabela[TAM_HASH];




// ----------------------- FUNÇÕES AUXILIARES -----------------------


// transforma tudo em minúsculo
void deixarMinusculo(char str[]){

    int i;

    for(i = 0; str[i]; i++){

        str[i] = tolower(str[i]);
    }
}



// remove pontuação da palavra
void limparPalavra(char str[]){

    char limpa[51];

    int i;
    int j = 0;

    for(i = 0; str[i]; i++){

        if(isalnum(str[i])){

            if(j < 50){

                limpa[j] = str[i];
                j++;
            }
        }
    }

    limpa[j] = '\0';

    strcpy(str, limpa);

    deixarMinusculo(str);
}



// verifica se o arquivo é txt
int ehTxt(char nome[]){

    char *ext = strrchr(nome, '.');

    if(ext != NULL && strcmp(ext, ".txt") == 0){

        return 1;
    }

    return 0;
}




// ----------------------- HASH -----------------------


// calcula posição da hash
int calcularHash(char str[]){

    int hash = 0;

    int i;

    for(i = 0; str[i]; i++){

        hash = hash * 31 + str[i];
    }

    if(hash < 0){

        hash = hash * -1;
    }

    return hash % TAM_HASH;
}



// procura palavra na hash
Palavra *buscarPalavra(char texto[]){

    int indice = calcularHash(texto);

    Palavra *aux = tabela[indice];


    while(aux != NULL){

        if(strcmp(aux->texto, texto) == 0){

            return aux;
        }

        aux = aux->prox;
    }

    return NULL;
}



// adiciona palavra na hash
void inserirPalavra(char texto[], char arquivo[], long offset){

    int indice = calcularHash(texto);


    // verifica se já existe
    Palavra *palavra = buscarPalavra(texto);


    // cria novo local
    Local *novoLocal = malloc(sizeof(Local));

    if(novoLocal == NULL){

        return;
    }

    strcpy(novoLocal->nomeArquivo, arquivo);

    novoLocal->offset = offset;

    novoLocal->prox = NULL;


    // se palavra já existir
    if(palavra != NULL){

        novoLocal->prox = palavra->locais;

        palavra->locais = novoLocal;

        return;
    }


    // cria nova palavra
    Palavra *novaPalavra = malloc(sizeof(Palavra));

    if(novaPalavra == NULL){

        free(novoLocal);

        return;
    }

    strcpy(novaPalavra->texto, texto);

    novaPalavra->locais = novoLocal;


    // encadeamento
    novaPalavra->prox = tabela[indice];

    tabela[indice] = novaPalavra;
}




// ----------------------- INDEXAÇÃO -----------------------


// processa um arquivo txt
void processarArquivo(char caminhoArquivo[]){

    FILE *fp = fopen(caminhoArquivo, "r");


    if(fp == NULL){

        return;
    }


    char palavra[51];

    int letra;

    int i = 0;

    long inicio;


    while((letra = fgetc(fp)) != EOF){

        if(isalnum(letra)){

            // CORREÇÃO: Pega a posição exata antes de ler o resto da palavra
            inicio = ftell(fp) - 1;

            i = 0;
            palavra[i++] = (char)letra;


            while((letra = fgetc(fp)) != EOF && isalnum(letra)){

                if(i < 50){

                    palavra[i++] = (char)letra;
                }
            }

            palavra[i] = '\0';


            limparPalavra(palavra);


            // só indexa palavras com 5 letras ou mais
            if(strlen(palavra) >= 5){

                inserirPalavra(palavra, caminhoArquivo, inicio);
            }
        }
    }

    fclose(fp);
}



// percorre pasta
void indexarPasta(char pasta[]){

    DIR *dir = opendir(pasta);


    if(dir == NULL){

        printf("Erro ao abrir pasta!\n");

        return;
    }


    struct dirent *arquivo;


    while((arquivo = readdir(dir)) != NULL){

        if(ehTxt(arquivo->d_name)){

            char caminhoCompleto[520];

            sprintf(caminhoCompleto, "%s/%s", pasta, arquivo->d_name);

            printf("Indexando: %s\n", arquivo->d_name);

            processarArquivo(caminhoCompleto);
        }
    }

    closedir(dir);
}




// ----------------------- BUSCA -----------------------


// mostra trecho do arquivo
void mostrarTrecho(char arquivo[], long offset){

    FILE *fp = fopen(arquivo, "r");


    if(fp == NULL){

        return;
    }


    char trecho[51]; // Ajustado para 50 caracteres como pede a prova


    // vai direto para posição
    fseek(fp, offset, SEEK_SET);


    int lidos = fread(trecho, sizeof(char), 50, fp);

    trecho[lidos] = '\0';

    // Limpa quebras de linha para o print ficar bonito
    for(int i = 0; i < lidos; i++) if(trecho[i] == '\n' || trecho[i] == '\r') trecho[i] = ' ';

    // CORREÇÃO: Formato de saída real-time pedido pelo professor
    char *soNome = strrchr(arquivo, '/');
    if(!soNome) soNome = strrchr(arquivo, '\\');
    if(soNome) soNome++; else soNome = arquivo;

    printf("%s - \"...%s...\"\n", soNome, trecho);


    fclose(fp);
}



// busca palavras pequenas direto nos arquivos
void buscarPalavraPequena(char pasta[], char termo[]){

    DIR *dir = opendir(pasta);


    if(dir == NULL){

        return;
    }


    struct dirent *arquivo;


    while((arquivo = readdir(dir)) != NULL){

        if(ehTxt(arquivo->d_name)){

            char caminhoArquivo[520];

            sprintf(caminhoArquivo, "%s/%s", pasta, arquivo->d_name);


            FILE *fp = fopen(caminhoArquivo, "r");


            if(fp == NULL){

                continue;
            }


            char palavra[51];

            int letra;

            int i = 0;

            long inicio;


            while((letra = fgetc(fp)) != EOF){

                if(isalnum(letra)){

                    inicio = ftell(fp) - 1;

                    i = 0;
                    palavra[i++] = (char)letra;


                    while((letra = fgetc(fp)) != EOF && isalnum(letra)){

                        if(i < 50){

                            palavra[i++] = (char)letra;
                        }
                    }

                    palavra[i] = '\0';


                    limparPalavra(palavra);


                    if(strcmp(palavra, termo) == 0){

                        mostrarTrecho(caminhoArquivo, inicio);
                    }
                }
            }

            fclose(fp);
        }
    }

    closedir(dir);
}



// busca principal
void buscarTermo(char pasta[], char termo[]){

    limparPalavra(termo);


    // palavras pequenas não ficam na hash
    if(strlen(termo) < 5){

        buscarPalavraPequena(pasta, termo);

        return;
    }


    Palavra *palavra = buscarPalavra(termo);


    if(palavra == NULL){

        printf("\nPalavra nao encontrada!\n");

        return;
    }


    Local *aux = palavra->locais;


    while(aux != NULL){

        mostrarTrecho(aux->nomeArquivo, aux->offset);

        aux = aux->prox;
    }
}




// libera memória
void liberarHash(){

    int i;


    for(i = 0; i < TAM_HASH; i++){

        Palavra *auxPalavra = tabela[i];


        while(auxPalavra != NULL){

            Local *auxLocal = auxPalavra->locais;


            while(auxLocal != NULL){

                Local *tempLocal = auxLocal;

                auxLocal = auxLocal->prox;

                free(tempLocal);
            }

            Palavra *tempP = auxPalavra;
            auxPalavra = auxPalavra->prox;

            free(tempP);
        }
        tabela[i] = NULL;
    }
}




// ----------------------- MAIN -----------------------


int main(){


    // inicia tabela com NULL
    int i;

    for(i = 0; i < TAM_HASH; i++){

        tabela[i] = NULL;
    }


    char pasta[260];

    char termo[51];


    printf("Digite o caminho da pasta: ");

    fgets(pasta, sizeof(pasta), stdin);

    pasta[strcspn(pasta, "\n")] = '\0';


    printf("\nIndexando arquivos...\n\n");


    indexarPasta(pasta);


    printf("\nIndexacao concluida!\n");


    while(1){

        printf("\nPesquise o termo (ou sair): ");

        fgets(termo, sizeof(termo), stdin);

        termo[strcspn(termo, "\n")] = '\0';


        if(strcmp(termo, "sair") == 0){

            break;
        }


        buscarTermo(pasta, termo);
        
        printf("\nPrecione Enter para continuar...");
        getchar();
    }


    liberarHash();

    printf("\nPrograma encerrado.\n");

    return 0;
}