//
// Created by tuzin on 12/31/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <definitions.h>
#include <funcs.h>
#include <io.h>

int main(const int argc, char *argv[]) {
        if (argc != 2) {
                extractor_usage();
                return 1;
        }
        GenInfo barcodeInfo = {-1};
        FILE *input_file = fopen(argv[1], "r");
        if (input_file == NULL) {
                fprintf(
                        stderr,
                        "ERRO: Algo deu errado ao tentar abrir o arquivo. "
                        "Confira o nome dele ou informe o caminho correto.\n");
                return 1;
        }

        char first_line[3];
        char st1[5];
        char st2[5];
        fscanf(input_file, "%s", first_line);
        if (strcmp(first_line, "P1") != 0) {
                fprintf(stderr,"ERRO: Tipo de arquivo inválido.\n");
                goto cleanup;
        }
        if (fscanf(input_file, "%s %s", st1, st2) == EOF) {
                fprintf(stderr,"ERRO: Tipo de arquivo inválido.\n"
                        "Não foi possível ler as dimensões do código de barras.\n");
                goto cleanup;
        };
        const int width = atoi(st1);
        if (width <= 0 || width > 1024) {
                fprintf(stderr,"ERRO: Tipo de arquivo inválido.\n"
                       "O arquivo é muito grande.\n");
                goto cleanup;
        }

        char* file_line = malloc(sizeof(char) * width + 1);
        if (file_line == NULL) {
                fprintf(stderr,"ERRO: Falha na alocação de memória.\n");
                goto cleanup;
        }

        while (barcodeInfo.margin == -1) {
                if (fscanf(input_file, "%s", file_line) == EOF) break;
                for (int i = 0; i < width; i++) {
                        if (file_line[i] == '1') {
                                barcodeInfo.margin = i;
                                break;
                        }
                }
        }
        if (barcodeInfo.margin == -1) {
                fprintf(stderr, "ERRO: Tipo de arquivo inválido.\n"
                       "O arquivo está vazio ou não existe código de barras para ser lido.\n");
                goto cleanup;
        }
        file_line[width] = '\0';
        if (fclose(input_file) == EOF) fprintf(stderr, "ERRO: Erro crítico ao fechar o arquivo.\n");
        input_file = NULL;

        const int barcode_width = width - (barcodeInfo.margin * 2) + 1;
        char *barcode_line = malloc(sizeof(char) * (barcode_width));
        if (barcode_line == NULL) {
                fprintf(stderr, "ERRO: Falha na alocação de memória.\n");
                goto cleanup;
        }
        int j = 0;
        for (int i = barcodeInfo.margin; i < strlen(file_line) - barcodeInfo.margin; i++) {
                barcode_line[j] = file_line[i];
                j++;
        }
        barcode_line[j] = '\0';

        free(file_line);
        file_line = NULL;

        for (int i = 0; i < barcode_line[i] != '\0'; i++) {
                if (barcode_line[i] == '0') {
                        barcodeInfo.area = i;
                        break;
                }
        }

        char* ean8_identifier = malloc((sizeof(char) * CODE_LEN) + 1);
        if (ean8_identifier == NULL) {
                fprintf(stderr, "ERRO: Falha na alocação de memória.\n");
                goto cleanup;
        }
        for (int i = 0; i < CODE_LEN; i++) {
                ean8_identifier[i] = barcode_line[i * barcodeInfo.area];
        }

        free(barcode_line);
        barcode_line = NULL;

        ean8_identifier[CODE_LEN] = '\0';

        if (strncmp(ean8_identifier, "101", 3) != 0 ||
                strncmp(ean8_identifier + CODE_LEN - 3, "101", 3) != 0) {
                fprintf(stderr, "ERRO: Tipo de arquivo inválido.\n"
                                "O início ou o fim do código de barras não foi encontrado. "
                                "O código de barras deve iniciar e finalizar com '101'.\n");
                goto cleanup;
        }

        if (strncmp(ean8_identifier + 31, "01010", 3) != 0) {
                fprintf(stderr, "ERRO: Tipo de arquivo inválido.\n"
                                "O meio do código de barras não foi encontrado. "
                                "O código de barras deve conter '01010' no meio.\n");
                goto cleanup;
        }

        char buffer[9];
        for (int i = 3; i < 32; i += 7) {
                memset(buffer, 0, sizeof(buffer));
                for (int j = 0; j < 7; j++) {
                        char temp[2] = {ean8_identifier[i + j], '\0'};
                        strcat(buffer, temp);
                }
                for (int k = 0; k < 10; k++) {
                        if (strcmp(buffer, l_codes[k]) == 0) {
                                char c = k + '0';
                                char temp_c[2] = {c, '\0'};
                                strcat(barcodeInfo.identifier, temp_c);
                        }
                }
        }

        for (int i = 36; i < 64; i += 7) {
                memset(buffer, 0, sizeof(buffer));
                for (int j = 0; j < 7; j++) {
                        char temp[2] = {ean8_identifier[i + j], '\0'};
                        strcat(buffer, temp);
                }
                for (int k = 0; k < 10; k++) {
                        if (strcmp(buffer, r_codes[k]) == 0) {
                                char c = k + '0';
                                char temp_c[2] = {c, '\0'};
                                strcat(barcodeInfo.identifier, temp_c);
                        }
                }
        }
        barcodeInfo.identifier[8] = '\0';


        char verifier = get_verification_digit(barcodeInfo.identifier);
        if (verifier != barcodeInfo.identifier[7]) {
                fprintf(stderr, "ERRO: Identificador %s de %s é inválido.\n "
                                "Para ser válido, o último dígito deveria ser %d.\n", barcodeInfo.identifier, argv[1], verifier);
                goto cleanup;
        }


        printf("Identificador: %s\n", barcodeInfo.identifier);

        free(ean8_identifier);
        return 0;

        cleanup:
        if (input_file) if (fclose(input_file) == EOF) fprintf(stderr, "ERRO: Erro crítico ao fechar o arquivo.\n");
        if (ean8_identifier) free(ean8_identifier);
        return 1;
}
