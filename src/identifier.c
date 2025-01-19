#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

// Função para calcular o dígito verificador
int calcularDigitoVerificador(int identificador) {
    int soma = 0;
    for (int i = 7; i >= 1; i--) {
        int digito = (identificador / (int)pow(10, i)) % 10;
        soma += digito * ((i % 2 == 1) ? 3 : 1);
    }

    int proximoMultiploDez = ((soma + 9) / 10) * 10; // Encontra o próximo múltiplo de 10
    return proximoMultiploDez - soma;
}

char* traduzirEAN8(int identificador) {
    char lcodes[10][8] = {
        "0001101", "0011001", "0010011", "0111101", "0100011",
        "0110001", "0101111", "0111011", "0110111", "0001011"
    };
    char rcodes[10][8] = {
        "1110010", "1100110", "1101100", "1000010", "1011100",
        "1001110", "1010000", "1000100", "1001000", "1110100"
    };

    // Código de início do EAN-8
    static char codigo[100] = "101";

    // Processa os primeiros 4 dígitos com L-code
    for (int i = 7; i >= 4; i--) {
        int digito = (identificador / (int)pow(10, i)) % 10;
        strcat(codigo, lcodes[digito]);
    }

    // Adiciona o código do meio
    strcat(codigo, "01010");

    // Processa os últimos 4 dígitos com R-code
    for (int i = 3; i >= 0; i--) {
        int digito = (identificador / (int)pow(10, i)) % 10;
        strcat(codigo, rcodes[digito]);
    }

    // Adiciona o código de fim
    strcat(codigo, "101");

    // Imprime o código de barras traduzido
    return codigo;
}

int main() {
    int identificador;
    bool valid_code = false;
    while (!valid_code) {
        printf("Digite um identificador de 8 dígitos: ");
        scanf("%d", &identificador);

        int digitoVerificadorEsperado = calcularDigitoVerificador(identificador);
        int digitoVerificadorAtual = identificador % 10;
        if (digitoVerificadorEsperado != digitoVerificadorAtual) {
            printf("Erro: Dígito verificador inválido.\n"
               "Para o código que você inseriu, o último digito deve ser: %d\n\n", digitoVerificadorEsperado);
            printf("Nova tentativa necessária.\n");
            continue;
        }
        valid_code = true;

    }

    printf("Identificador válido.\n");
    char* codigoDeBarras = traduzirEAN8(identificador);
    printf("%s\n", codigoDeBarras);

    int margem = 4;
    int area = 3;
    int altura = 50;

    int altura_total = altura + (margem*2);
    int largura_total = (67*area) + (margem*2);

    char* linha_codigo = malloc(sizeof(char) * (largura_total + 1));
    for (int i = 0; i < strlen(codigoDeBarras); i++) {
        for (int j = 0; j < area; j++) {
            linha_codigo[i * area + j] = codigoDeBarras[i];
        }
    }
    linha_codigo[largura_total] = '\0';
    printf("%s\n", linha_codigo);

    char* linha_margem = malloc(sizeof(char) * (largura_total + 1));
    for (int i = 0; i < largura_total; i++) {
        linha_margem[i] = '0';
    }
    linha_margem[largura_total] = '\0';
    printf("%s\n", linha_margem);

    printf("\n======================INICIO DO ARQUIVO PBM======================\n");
    printf("P1\n");
    printf("%d %d\n", largura_total, altura_total);

    for (int i = 0; i < margem; i++) {
        printf("%s\n", linha_margem);
    }

    char* coluna_margem = malloc(sizeof(char) * (margem + 1));
    for (int i = 0; i < margem; i++) {
        coluna_margem[i] = '0';
    }

    for (int i = 0; i < altura; i++) {
        printf("%s", coluna_margem);
        printf("%s", linha_codigo);
        printf("%s\n", coluna_margem);
    }

    for (int i = 0; i < margem; i++) {
        printf("%s\n", linha_margem);
    }

    free(coluna_margem);
    free(linha_margem);
    free(linha_codigo);


    return 0;
}
