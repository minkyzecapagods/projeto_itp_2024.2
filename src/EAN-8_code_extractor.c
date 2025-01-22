//
// Created by tuzin on 12/31/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/definitions.h"
#include "../include/funcs.h"
#include "../include/io.h"

int main(const int argc, char *argv[]) {
        if (argc != 2) {
                extractor_usage();
                return 1;
        }

        FILE *input_file = fopen(argv[1], "r");
        if (input_file == NULL) {
                fprintf(
                        stderr,
                        "ERRO: Algo deu errado ao tentar abrir o arquivo. "
                        "Confira o nome dele ou informe o caminho correto.\n");
                return 1;
        }

        PBMImage pbm_image = get_pbm_info(input_file);

        if (strncmp(pbm_image.ean8_code, "101", 3) != 0 ||
                strncmp(pbm_image.ean8_code + CODE_LEN - 3, "101", 3) != 0) {
                fprintf(stderr, "ERRO: Tipo de arquivo inválido.\n"
                                "O início ou o fim do código de barras não foi encontrado. "
                                "O código de barras deve iniciar e finalizar com '101'.\n");
                goto cleanup;
        }

        if (strncmp(pbm_image.ean8_code + 31, "01010", 3) != 0) {
                fprintf(stderr, "ERRO: Tipo de arquivo inválido.\n"
                                "O meio do código de barras não foi encontrado. "
                                "O código de barras deve conter '01010' no meio.\n");
                goto cleanup;
        }

        char *identifier = to_identifier(pbm_image.ean8_code);

        char verifier = get_verification_digit(identifier);
        if (verifier != identifier[7]) {
                fprintf(stderr, "ERRO: Identificador %s de %s é inválido.\n "
                                "Para ser válido, o último dígito deveria ser %d.\n", identifier, argv[1], verifier);
                goto cleanup;
        }


        printf("Identificador: %s\n", identifier);

        if (pbm_image.ean8_code) free(pbm_image.ean8_code);
        return 0;

        cleanup:
        if (pbm_image.ean8_code) free(pbm_image.ean8_code);
        return 1;
}