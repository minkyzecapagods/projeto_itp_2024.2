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
    return pbm_image;
}

