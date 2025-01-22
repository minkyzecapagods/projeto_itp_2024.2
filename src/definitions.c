//
// Created by zecapagods on 1/22/25.
//

#include "../include/definitions.h"

// Estrutura que armazena os códigos correspondentes para os dígitos de 0 a 9, à esquerda do código de barras.
const char l_codes[10][8] = {
    "0001101", "0011001", "0010011", "0111101", "0100011",
    "0110001", "0101111", "0111011", "0110111", "0001011"
};

// Estrutura que armazena os códigos correspondentes para os dígitos de 0 a 9, à direita do código de barras.
const char r_codes[10][8] = {
    "1110010", "1100110", "1101100", "1000010", "1011100",
    "1001110", "1010000", "1000100", "1001000", "1110100"
};