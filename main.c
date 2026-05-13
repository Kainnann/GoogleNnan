#include <stdio.h>
#include <string.h>
#include "indexador.h"

int main(){
    inicializarTabela();
    char pasta[260], termo[51];

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

        if(strcmp(termo, "sair") == 0) break;

        buscarTermo(pasta, termo);
        printf("\nPesquise o termo (ou sair): ");
    }

    liberarHash();
    printf("\nPrograma encerrado.\n");
    return 0;
}