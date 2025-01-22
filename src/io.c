//
// Created by zecapagods on 1/22/25.
//
#include <stdio.h>
#include <stdlib.h>

#include "../include/io.h"
#include "../include/definitions.h"

void generator_usage() {
    printf("LEITOR DE CÓDIGO DE BARRAS\n");
    printf("\tExtrai o identificador do código de barras no arquivo .pbm informado.\n");
    printf("\tPara acessar arquivos dentro da pasta 'barcode-output', inserir o caminho '../barcode-output/' antes do nome do arquivo.\n");
    printf("Uso:\n");
    printf("\t./EAN-8_code_extractor <nome_do_arquivo>\n");
}

void extractor_usage() {
    printf("LEITOR DE CÓDIGO DE BARRAS\n");
    printf("\tExtrai o identificador do código de barras no arquivo .pbm informado.\n");
    printf("\tPara acessar arquivos dentro da pasta 'barcode-output', inserir o caminho '../barcode-output/' antes do nome do arquivo.\n");
    printf("Uso:\n");
    printf("\t./EAN-8_code_extractor <nome_do_arquivo>\n");
}

void check_size(int width, int height) {
    if (height > MAX_SIZE || width > MAX_SIZE) {
        fprintf(stderr, "ERRO DE ENTRADA: As dimensões da imagem excedem o limite permitido.\n");
        exit(1);
    }
}

void check_file_exists(char* filename) {
    if (fopen(filename, "r") != NULL) {
        fprintf(stderr, "AVISO O arquivo '%s' já existe.\n"
                        "Deseja sobrescrever a pasta existente? (s/n) ", filename);
        char r;
        while (1) {
            r = getchar();
            if (r == 's' || r == 'S') break;
            if (r == 'n' || r == 'N') {
                printf("Operação cancelada. O arquivo não será sobrescrito.\n");
                exit(1);
            }
            printf("Entrada inválida. Por favor, digite 's' para sobrescrever ou 'n' para cancelar: ");
            while ((r = getchar()) != '\n' && r != EOF);
        }
    }
}