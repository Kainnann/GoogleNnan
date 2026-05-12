#ifndef INDEXADOR_H
#define INDEXADOR_H

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

// Funções que o main vai usar
void inicializarTabela();
void indexarPasta(char pasta[]);
void buscarTermo(char pasta[], char termo[]);
void liberarHash();

#endif