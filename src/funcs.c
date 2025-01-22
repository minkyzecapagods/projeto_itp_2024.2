//
// Created by zecapagods on 1/22/25.
//
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/funcs.h"
#include "../include/definitions.h"
#include "../include/io.h"

// Função para calcular o dígito verificador

char get_verification_digit(const char *id) {
    int sum = 0;
    for (int i = 1; i < 8; i++) {
        sum += (id[i-1] - '0') * ((i % 2 == 1) ? 3 : 1);
    }
    int next_mul_ten = ((sum + 9) / 10) * 10; // Encontra o próximo múltiplo de 10
    int digit = next_mul_ten - sum;
    return digit + '0';
}

char* to_ean8(const char *id) {

    // Código de início do EAN-8
    static char code_line[CODE_LEN] = "101";
    int digit;

    // Processa os primeiros 4 dígitos com L-code
    for (int i = 0; i < 4; i++) {
        digit = id[i] - '0';
        strcat(code_line, l_codes[digit]);
    }

    // Adiciona o código do meio
    strcat(code_line, "01010");

    // Processa os últimos 4 dígitos com R-code
    for (int i = 4; i < 8; i++) {
        digit = id[i] - '0';
        strcat(code_line, r_codes[digit]);
    }

    // Adiciona o código de fim
    strcat(code_line, "101");

    // Imprime o código de barras traduzido
    return code_line;
}

PBMImage create_pbm_info(const GenInfo info) {
    PBMImage pbm_image;
    pbm_image.height  = info.height + (info.margin * 2);
    pbm_image.width = (CODE_LEN * info.area) + (info.margin * 2);
    check_size(pbm_image.width, pbm_image.height);

    sprintf(pbm_image.filename, "%s%s%s", "../barcode-output/", info.title, ".pbm");
    check_file_exists(pbm_image.filename);

    pbm_image.ean8_code = to_ean8(info.identifier);
    pbm_image.barcode_line = create_barcode_line(info.area, pbm_image.width, pbm_image.ean8_code);
    return pbm_image;
}

char* create_barcode_line(const int area, const int width, char* ean8_code) {
    char* barcode_line = malloc(sizeof(char) * (width + 1));
    if (barcode_line == NULL) {
        fprintf(stderr, "ERRO: Falha na alocação de memória.\n");
        exit(1);
    }

    for (int i = 0; i < CODE_LEN; i++) {
        for (int j = 0; j < area; j++) {
            barcode_line[i * area + j] = ean8_code[i];
        }
    }
    barcode_line[width] = '\0';
    return barcode_line;
}

void create_pbm_image(PBMImage pbm_image, int height, int margin) {
        // margin_line é a linha que servirá como margem superior e inferior no arquivo PBM
        char* margin_line = malloc(sizeof(char) * (pbm_image.width + 1));
        if (margin_line == NULL) {
                fprintf(stderr, "ERRO: Falha na alocação de memória.\n");
                free(pbm_image.barcode_line);
                exit(1);
        }
        for (int i = 0; i < pbm_image.width; i++) margin_line[i] = '0';
        margin_line[pbm_image.width] = '\0';

        // margin_column representa as margens laterais do arquivo PBM
        char* margin_column = malloc(sizeof(char) * (margin + 1));
        if (margin_column == NULL) {
                fprintf(stderr, "ERRO: Falha na alocação de memória.\n");
                free(pbm_image.barcode_line);
                free(margin_line);
                exit(1);
        }
        for (int i = 0; i < margin; i++) margin_column[i] = '0';


        FILE *output_file = fopen(pbm_image.filename, "w");
        if (output_file == NULL) {
                fprintf(stderr, "ERRO: Algo deu errado ao abrir o arquivo.\n");
                free(pbm_image.barcode_line);
                free(margin_line);
                free(margin_column);
                exit(1);
        }

        fprintf(output_file, "P1\n");
        fprintf(output_file, "%d %d\n", pbm_image.width, pbm_image.height);

        // Imprimindo margens superiores
        for (int i = 0; i < margin; i++) fprintf(output_file, "%s\n", margin_line);

        // Imprimindo os códigos já expandidos e com as margens, com a altura informada
        for (int i = 0; i < height; i++) {
                fprintf(output_file, "%s", margin_column);
                fprintf(output_file, "%s", pbm_image.barcode_line);
                fprintf(output_file,"%s\n", margin_column);
        }

        // Imprimindo margens inferiores
        for (int i = 0; i < margin; i++) fprintf(output_file,"%s\n", margin_line);

        fclose(output_file);
        free(margin_column);
        free(pbm_image.barcode_line);
        free(margin_line);
}