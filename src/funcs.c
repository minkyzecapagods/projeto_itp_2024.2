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

PBMImage get_pbm_info(FILE* input_file) {

    PBMImage pbm_image = {check_barcode_file(input_file)};

    pbm_image.barcode_line = malloc(sizeof(char) * pbm_image.width + 1);
    if (pbm_image.barcode_line == NULL) {
        fprintf(stderr,"ERRO: Falha na alocação de memória.\n");
        goto cleanup;
    }

    int margin = -1;
    while (margin == -1) {
        if (fscanf(input_file, "%s", pbm_image.barcode_line) == EOF) break;
        for (int i = 0; i < pbm_image.width; i++) {
            if (pbm_image.barcode_line[i] == '1') {
                margin = i;
                break;
            }
        }
    }
    if (margin == -1) {
        fprintf(stderr, "ERRO: Tipo de arquivo inválido.\n"
               "O arquivo está vazio ou não existe código de barras para ser lido.\n");
        goto cleanup;
    }

    pbm_image.barcode_line[pbm_image.width] = '\0';
    const int barcode_width = pbm_image.width - (margin * 2) + 1;
    char *no_margin_line = malloc(sizeof(char) * (barcode_width));
    if (no_margin_line == NULL) {
        fprintf(stderr, "ERRO: Falha na alocação de memória.\n");
        goto cleanup;
    }
    int j = 0;
    for (int i = margin; i < pbm_image.width - margin; i++) {
        no_margin_line[j] = pbm_image.barcode_line[i];
        j++;
    }

    free (pbm_image.barcode_line);
    pbm_image.barcode_line = NULL;
    if (fclose(input_file) == EOF) fprintf(stderr, "ERRO: Erro crítico ao fechar o arquivo.\n");
    input_file = NULL;

    pbm_image.ean8_code = from_barcode(no_margin_line);

    return pbm_image;

    cleanup:
      if(pbm_image.barcode_line) free(pbm_image.barcode_line);
      if (fclose(input_file) == EOF) fprintf(stderr, "ERRO: Erro crítico ao fechar o arquivo.\n");
      exit(1);
}

char* from_barcode(char* barcode) {
    int area = 0;
    for (int i = 0; i < barcode[i] != '\0'; i++) {
        if (barcode[i] == '0') {
            area = i;
            break;
        }
    }
    char* ean8_identifier = malloc((sizeof(char) * CODE_LEN) + 1);
    if (ean8_identifier == NULL) {
        fprintf(stderr, "ERRO: Falha na alocação de memória.\n");
        free(barcode);
        exit(1);
    }
    for (int i = 0; i < CODE_LEN; i++) {
        ean8_identifier[i] = barcode[i * area];
    }
    ean8_identifier[CODE_LEN] = '\0';

    free(barcode);
    barcode = NULL;

    return ean8_identifier;
}

char* to_identifier(char* ean8_code) {
  static char identifier[9];
    char buffer[9];
    for (int i = 3; i < 32; i += 7) {
        memset(buffer, 0, sizeof(buffer));
        for (int j = 0; j < 7; j++) {
            char temp[2] = {ean8_code[i + j], '\0'};
            strcat(buffer, temp);
        }
        for (int k = 0; k < 10; k++) {
            if (strcmp(buffer, l_codes[k]) == 0) {
                char c = k + '0';
                char temp_c[2] = {c, '\0'};
                strcat(identifier, temp_c);
            }
        }
    }

    for (int i = 36; i < 64; i += 7) {
        memset(buffer, 0, sizeof(buffer));
        for (int j = 0; j < 7; j++) {
            char temp[2] = {ean8_code[i + j], '\0'};
            strcat(buffer, temp);
        }
        for (int k = 0; k < 10; k++) {
            if (strcmp(buffer, r_codes[k]) == 0) {
                char c = k + '0';
                char temp_c[2] = {c, '\0'};
                strcat(identifier, temp_c);
            }
        }
    }
    identifier[8] = '\0';
    return identifier;
}