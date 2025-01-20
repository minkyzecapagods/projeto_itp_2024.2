//
// Created by tuzin on 12/31/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

void usage() {
        printf("BARCODE READER\n");
        printf("\tExtracts the barcode indentifier from the input PBM file.\n");
        printf("Usage:\n");
        printf("\t/EAN-8_code_extractor <input_file>\n");
}



int main(const int argc, char *argv[]) {

        char lcodes[10][8] = {
                "0001101", "0011001", "0010011", "0111101", "0100011",
                "0110001", "0101111", "0111011", "0110111", "0001011"
            };
        char rcodes[10][8] = {
                "1110010", "1100110", "1101100", "1000010", "1011100",
                "1001110", "1010000", "1000100", "1001000", "1110100"
            };

        FILE *input = fopen("../imagem.pbm", "r");
        // if (input == NULL) {
        // Por algum motivo NULL provoca um erro de unknown identifier

        char* linha = malloc(sizeof(char) * 1024);
        char validador[3];
        int largura, altura, margem;
        fscanf(input, "%s", validador);
        if (strcmp(validador, "P1") != 0) {
                printf("Erro ao abrir o arquivo\n");
                return 1;
        }
        fscanf(input, "%d %d", &largura, &altura);
        printf("Largura: %d\n", largura);
        printf("Altura: %d\n", altura);

        bool codigo_encontrado = false;
        while (!codigo_encontrado) {
                fscanf(input, "%s", linha);
                int i = 0;
                for (i; i < largura; i++) {
                        if (linha[i] == '1') {
                                margem = i;
                                codigo_encontrado = true;
                                break;
                        }
                }

        }
        linha[largura] = '\0';

        printf("Margem: %d\n", margem);
        printf("Conteudo da linha com margem: %s\n", linha);
        char* linha_sem_margem = malloc(sizeof(char) * 1024);
        int j = 0;
        for (int i = margem; i < strlen(linha) - margem; i++) {
                linha_sem_margem[j] = linha[i];
                j++;
        }
        linha_sem_margem[j] = '\0';
        printf("Conteúdo da linha sem margem: %s\n", linha_sem_margem);
        int area = 0;
        for (int i = 0; i < linha_sem_margem[i] != '\0'; i++) {
                if (linha_sem_margem[i] == '0') {
                     area = i;
                        break;
                }
        }

        int comprimento_expandido = strlen(linha_sem_margem);
        int comprimento_original = comprimento_expandido / area;

        char* string_original = malloc((sizeof(char) * comprimento_original) + 1);
        for (int i = 0; i < comprimento_original; i++) {
                string_original[i] = linha_sem_margem[i * area];
        }
        string_original[comprimento_original] = '\0';
        printf("String: %s\n", string_original);

        char trecho_codigo[8] = "";
        char codigo_final[9] = "";
        for (int i = 3; i < 32; i += 7) {
                memset(trecho_codigo, 0, sizeof(trecho_codigo));
                for (int j = 0; j < 7; j++) {
                        char temp[2] = {string_original[i + j], '\0'};
                        strcat(trecho_codigo, temp);
                }
                for (int k = 0; k < 10; k++) {
                        if (strcmp(trecho_codigo, lcodes[k]) == 0) {
                                char c = k + '0';
                                char temp_c[2] = {c, '\0'};
                                strcat(codigo_final, temp_c);
                        }
                }
        }

        for (int i = 36; i < 64; i += 7) {
                memset(trecho_codigo, 0, sizeof(trecho_codigo));
                for (int j = 0; j < 7; j++) {
                        char temp[2] = {string_original[i + j], '\0'};
                        strcat(trecho_codigo, temp);
                }
                for (int k = 0; k < 10; k++) {
                        if (strcmp(trecho_codigo, rcodes[k]) == 0) {
                                char c = k + '0';
                                char temp_c[2] = {c, '\0'};
                                strcat(codigo_final, temp_c);
                        }
                }
        }

        printf("Codigo final: %s\n", codigo_final);

        free(string_original);
        free(linha_sem_margem);
        free(linha);
        fclose(input);


        return 0;
}
