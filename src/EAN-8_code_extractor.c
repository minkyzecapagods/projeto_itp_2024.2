//
// Created by tuzin on 12/31/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char calcularDigitoVerificador(const char *id) {
        int soma = 0;
        for (int i = 1; i < 8; i++) {
                soma += (id[i-1] - '0') * ((i % 2 == 1) ? 3 : 1);
        }
        int proximoMultiploDez = ((soma + 9) / 10) * 10; // Encontra o próximo múltiplo de 10
        int resultado = proximoMultiploDez - soma;
        return resultado + '0';
}

void usage() {
        printf("BARCODE READER\n");
        printf("\tExtracts the barcode indentifier from the input PBM file.\n");
        printf("Usage:\n");
        printf("\t/EAN-8_code_extractor <input_file_path>\n");
}

typedef struct {
        int margin;
        int area;
        int height;
        char identifier[9];
        char title[30];
} GENInfo;


int main(const int argc, char *argv[]) {
        const char lcodes[10][8] = {
                "0001101", "0011001", "0010011", "0111101", "0100011",
                "0110001", "0101111", "0111011", "0110111", "0001011"
        };
        const char rcodes[10][8] = {
                "1110010", "1100110", "1101100", "1000010", "1011100",
                "1001110", "1010000", "1000100", "1001000", "1110100"
        };
        if (argc != 2) {
                usage();
                return 1;
        }
        GENInfo barcodeInfo = {-1};
        strcpy(barcodeInfo.title, argv[1]);
        FILE *input = fopen(barcodeInfo.title, "r");
        if (input == NULL) {
                fprintf(
                        stderr,
                        "ERROR: Something went wrong when trying to open the input file. "
                        "Check the name or specify the correct file path.\n");
                return 1;
        }

        char validador[3];
        char st1[5];
        char st2[5];
        fscanf(input, "%s", validador);
        if (strcmp(validador, "P1") != 0) {
                fprintf(stderr,"ERROR: Wrong file format.\n");
                goto cleanup;
        }
        if (fscanf(input, "%s %s", st1, st2) == EOF) {
                fprintf(stderr,"ERROR: Wrong file format.\n"
                        "Couldn't read the file dimensions.\n");
                goto cleanup;
        };
        const int width = atoi(st1);
        if (width <= 0 || width > 1024) {
                fprintf(stderr,"ERROR: Wrong file format.\n"
                       "The file is too big.\n");
                goto cleanup;
        }

        char* linha = malloc(sizeof(char) * width + 1);
        if (linha == NULL) {
                fprintf(stderr,"ERROR: Memory allocation failed.\n");
                goto cleanup;
        }

        while (barcodeInfo.margin == -1) {
                if (fscanf(input, "%s", linha) == EOF) break;
                for (int i = 0; i < width; i++) {
                        if (linha[i] == '1') {
                                barcodeInfo.margin = i;
                                break;
                        }
                }
        }
        if (barcodeInfo.margin == -1) {
                fprintf(stderr, "ERROR: Wrong file format.\n"
                       "The file is blank, there is no barcode to be read.\n");
                goto cleanup;
        }
        linha[width] = '\0';
        fclose(input);
        input = NULL;

        const int comprimento_expandido = width - (barcodeInfo.margin * 2) + 1;
        char *linha_sem_margem = malloc(sizeof(char) * comprimento_expandido);
        if (linha_sem_margem == NULL) {
                fprintf(stderr, "ERROR: Memory allocation failed.\n");
                goto cleanup;
        }
        int j = 0;
        for (int i = barcodeInfo.margin; i < strlen(linha) - barcodeInfo.margin; i++) {
                linha_sem_margem[j] = linha[i];
                j++;
        }
        linha_sem_margem[j] = '\0';

        free(linha);
        linha = NULL;

        for (int i = 0; i < linha_sem_margem[i] != '\0'; i++) {
                if (linha_sem_margem[i] == '0') {
                        barcodeInfo.area = i;
                        break;
                }
        }
        const int comprimento_original = comprimento_expandido / barcodeInfo.area;

        char* string_original = malloc((sizeof(char) * comprimento_original) + 1);
        if (string_original == NULL) {
                fprintf(stderr, "ERROR: Memory allocation failed.\n");
                goto cleanup;
        }
        for (int i = 0; i < comprimento_original; i++) {
                string_original[i] = linha_sem_margem[i * barcodeInfo.area];
        }

        free(linha_sem_margem);
        linha_sem_margem = NULL;

        string_original[comprimento_original] = '\0';

        if (strncmp(string_original, "101", 3) != 0 ||
                strncmp(string_original + comprimento_original - 3, "101", 3) != 0) {
                fprintf(stderr, "ERROR: Wrong file format.\n"
                                "There is no beginning or end marker. "
                                "The barcode should start and finish with a '101' sequence.\n");
                goto cleanup;
        }

        if (strncmp(string_original + 31, "01010", 3) != 0) {
                fprintf(stderr, "ERROR: Wrong file format.\n"
                                "There is no middle marker. "
                                "The middle of the barcode should be marked by a '01010' sequence.\n");
                goto cleanup;
        }

        char buffer[9];
        for (int i = 3; i < 32; i += 7) {
                memset(buffer, 0, sizeof(buffer));
                for (int j = 0; j < 7; j++) {
                        char temp[2] = {string_original[i + j], '\0'};
                        strcat(buffer, temp);
                }
                for (int k = 0; k < 10; k++) {
                        if (strcmp(buffer, lcodes[k]) == 0) {
                                char c = k + '0';
                                char temp_c[2] = {c, '\0'};
                                strcat(barcodeInfo.identifier, temp_c);
                        }
                }
        }

        for (int i = 36; i < 64; i += 7) {
                memset(buffer, 0, sizeof(buffer));
                for (int j = 0; j < 7; j++) {
                        char temp[2] = {string_original[i + j], '\0'};
                        strcat(buffer, temp);
                }
                for (int k = 0; k < 10; k++) {
                        if (strcmp(buffer, rcodes[k]) == 0) {
                                char c = k + '0';
                                char temp_c[2] = {c, '\0'};
                                strcat(barcodeInfo.identifier, temp_c);
                        }
                }
        }
        barcodeInfo.identifier[8] = '\0';


        char verificador = calcularDigitoVerificador(barcodeInfo.identifier);
        if (verificador != barcodeInfo.identifier[7]) {
                fprintf(stderr, "ERROR: Wrong file format.\n"
                                "Invalid barcode. The identifier should end with the number %d.\n", verificador);
                goto cleanup;
        }


        printf("Codigo final: %s\n", barcodeInfo.identifier);

        free(string_original);
        return 0;

        cleanup:
        if (input) if (fclose(input) == EOF) fprintf(stderr, "ERROR: Erro crítico ao fechar o arquivo.\n");
        if (string_original) free(string_original);
        return 1;
}
