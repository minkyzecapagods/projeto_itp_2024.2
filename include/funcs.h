//
// Created by zecapagods on 1/22/25.
//

#ifndef FUNCS_H
#define FUNCS_H

#include "definitions.h"

// Função para calcular o dígito verificador

char get_verification_digit(const char *id);

char* to_ean8(const char *id);

PBMImage create_pbm_info(const GenInfo info);

char* create_barcode_line(const int area, const int width, char* ean8_code);

void create_pbm_image(PBMImage pbm_image, int height, int margin);

#endif //FUNCS_H
