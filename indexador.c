#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include "indexador.h"

// tabela hash
Palavra *tabela[TAM_HASH];

// transforma tudo em minúsculo
static void deixarMinusculo(char str[]){
    int i;
    for(i = 0; str[i]; i++) str[i] = tolower(str[i]);
}

// remove pontuação da palavra
static void limparPalavra(char str[]){
    char limpa[51];
    int i, j = 0;
    for(i = 0; str[i]; i++){
        if(isalnum(str[i])){
            if(j < 50) limpa[j++] = str[i];
        }
    }
    limpa[j] = '\0';
    strcpy(str, limpa);
    deixarMinusculo(str);
}

// verifica se o arquivo é txt
static int ehTxt(char nome[]){
    char *ext = strrchr(nome, '.');
    return (ext != NULL && strcmp(ext, ".txt") == 0);
}

// calcula posição da hash
static int calcularHash(char str[]){
    int hash = 0;
    for(int i = 0; str[i]; i++) hash = hash * 31 + str[i];
    if(hash < 0) hash *= -1;
    return hash % TAM_HASH;
}

void inicializarTabela() {
    for (int i = 0; i < TAM_HASH; i++) tabela[i] = NULL;
}

// procura palavra na hash
static Palavra *buscarPalavra(char texto[]){
    int indice = calcularHash(texto);
    Palavra *aux = tabela[indice];
    while(aux != NULL){
        if(strcmp(aux->texto, texto) == 0) return aux;
        aux = aux->prox;
    }
    return NULL;
}

// adiciona palavra na hash
static void inserirPalavra(char texto[], char arquivo[], long offset){
    int indice = calcularHash(texto);
    Palavra *palavra = buscarPalavra(texto);
    Local *novoLocal = malloc(sizeof(Local));
    if(novoLocal == NULL) return;

    strcpy(novoLocal->nomeArquivo, arquivo);
    novoLocal->offset = offset;
    novoLocal->prox = NULL;

    if(palavra != NULL){
        novoLocal->prox = palavra->locais;
        palavra->locais = novoLocal;
        return;
    }

    Palavra *novaPalavra = malloc(sizeof(Palavra));
    if(novaPalavra == NULL) { free(novoLocal); return; }
    strcpy(novaPalavra->texto, texto);
    novaPalavra->locais = novoLocal;
    novaPalavra->prox = tabela[indice];
    tabela[indice] = novaPalavra;
}

// processa um arquivo txt
static void processarArquivo(char caminhoArquivo[]){
    FILE *fp = fopen(caminhoArquivo, "r");
    if(fp == NULL) return;
    char palavra[51];
    int letra, i;
    long inicio;

    while((letra = fgetc(fp)) != EOF){
        if(isalnum(letra)){
            inicio = ftell(fp) - 1;
            i = 0;
            palavra[i++] = (char)letra;
            while((letra = fgetc(fp)) != EOF && isalnum(letra)){
                if(i < 50) palavra[i++] = (char)letra;
            }
            palavra[i] = '\0';
            limparPalavra(palavra);
            if(strlen(palavra) >= 5) inserirPalavra(palavra, caminhoArquivo, inicio);
        }
    }
    fclose(fp);
}

void indexarPasta(char pasta[]){
    DIR *dir = opendir(pasta);
    if(dir == NULL){ printf("Erro ao abrir pasta!\n"); return; }
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

// mostra trecho do arquivo
static void mostrarTrecho(char arquivo[], long offset){
    FILE *fp = fopen(arquivo, "r");
    if(fp == NULL) return;
    char trecho[51];
    fseek(fp, offset, SEEK_SET);
    int lidos = fread(trecho, sizeof(char), 50, fp);
    trecho[lidos] = '\0';
    for(int i = 0; i < lidos; i++) if(trecho[i] == '\n' || trecho[i] == '\r') trecho[i] = ' ';
    
    char *soNome = strrchr(arquivo, '/');
    if(!soNome) soNome = strrchr(arquivo, '\\');
    if(soNome) soNome++; else soNome = arquivo;
    printf("%s - \"...%s...\"\n", soNome, trecho);
    fclose(fp);
}

// busca principal

// busca palavras pequenas fora da hash
static void buscarSobDemanda(char pasta[], char termo[]){

    DIR *dir = opendir(pasta);

    if(dir == NULL){

        return;
    }

    struct dirent *arquivo;

    int encontrou = 0;


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

            int i;

            long inicio;


            while((letra = fgetc(fp)) != EOF){

                if(isalnum(letra)){

                    inicio = ftell(fp) - 1;

                    i = 0;

                    palavra[i++] = letra;


                    while((letra = fgetc(fp)) != EOF && isalnum(letra)){

                        if(i < 50){

                            palavra[i++] = letra;
                        }
                    }

                    palavra[i] = '\0';


                    char temp[51];

                    strcpy(temp, palavra);

                    limparPalavra(temp);


                    if(strcmp(temp, termo) == 0){

                        mostrarTrecho(caminhoArquivo, inicio);

                        encontrou = 1;
                    }
                }
            }

            fclose(fp);
        }
    }

    closedir(dir);


    if(!encontrou){

        printf("\nPalavra nao encontrada!\n");
    }
}

// busca principal
void buscarTermo(char pasta[], char termo[]){

    limparPalavra(termo);


    // palavras pequenas ficam fora da hash
    if(strlen(termo) < 5){

        printf("\nBuscando termo curto fora da hash...\n");

        buscarSobDemanda(pasta, termo);

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

void liberarHash(){
    for(int i = 0; i < TAM_HASH; i++){
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