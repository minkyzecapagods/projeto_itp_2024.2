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

#endif //FUNCS_H
