//
// Created by tuzin on 12/31/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void usage() {
        printf("BARCODE READER\n");
        printf("\tExtracts the barcode indentifier from the input PBM file.\n");
        printf("Usage:\n");
        printf("\t/EAN-8_code_extractor <input_file>\n");
}

int main() {

        FILE *input = fopen("../imagem.pbm", "r");
        // if (input == NULL) {
        // Por algum motivo NULL provoca um erro de unknown identifier

        char* linha = malloc(sizeof(linha) * 1024);
        int largura, altura;

        return 0;
}
