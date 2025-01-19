#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

// Função para calcular o dígito verificador
int calcularDigitoVerificador(int identificador) {
    int soma = 0;
    for (int i = 7; i >= 1; i--) {
        int divisor_por_posicao = (int)pow(10, i);
        int digito = (identificador / divisor_por_posicao) % 10;
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
        int divisor_por_posicao = (int)pow(10, i);
        int digito = (identificador / divisor_por_posicao) % 10;
        strcat(codigo, lcodes[digito]);
    }

    // Adiciona o código do meio
    strcat(codigo, "01010");

    // Processa os últimos 4 dígitos com R-code
    for (int i = 3; i >= 0; i--) {
        int divisor_por_posicao = (int)pow(10, i);
        int digito = (identificador / divisor_por_posicao) % 10;
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
    // Loop para ficar pedindo o código até ele ser válido
    while (!valid_code) {
        printf("Digite um identificador de 8 dígitos: ");
        scanf("%d", &identificador);

        // A partir do 7 primeiros dígitos do código, calcula qual deve ser o digito verificador
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

    // Definição de parâmetros do arquivo PBM
    int margem = 4;
    int area = 3;
    int altura = 50;

    // Cálculo de parâmetros baseado nos parâmetros já fornecidos
    int altura_total = altura + (margem*2);
    int largura_total = (67*area) + (margem*2);

    // linha_codigo é a codificação das barras expandida baseada no parâmetro área
    char* linha_codigo = malloc(sizeof(char) * (largura_total + 1));
    if (linha_codigo == NULL) {
        fprintf(stderr, "Erro: Falha na alocação de memória para linha_codigo.\n");
        return 1;
    }

    // Expandindo o código de barras conforme a área
    for (int i = 0; i < strlen(codigoDeBarras); i++) {
        for (int j = 0; j < area; j++) {
            linha_codigo[i * area + j] = codigoDeBarras[i];
        }
    }
    linha_codigo[largura_total] = '\0';

    // linha_margem é a linha que servirá como margem superior e inferior no arquivo PBM
    char* linha_margem = malloc(sizeof(char) * (largura_total + 1));
    if (linha_margem == NULL) {
        fprintf(stderr, "Erro: Falha na alocação de memória para linha_margem.\n");
        free(linha_codigo);
        return 1;
    }
    for (int i = 0; i < largura_total; i++) {
        linha_margem[i] = '0';
    }
    linha_margem[largura_total] = '\0';

    printf("\n======================INICIO DO ARQUIVO PBM======================\n");
    printf("P1\n");
    printf("%d %d\n", largura_total, altura_total);

    // Imprimindo margens superiores
    for (int i = 0; i < margem; i++) {
        printf("%s\n", linha_margem);
    }

    // coluna_margem representa as margens laterais do arquivo PBM
    char* coluna_margem = malloc(sizeof(char) * (margem + 1));
    if (coluna_margem == NULL) {
        fprintf(stderr, "Erro: Falha na alocação de memória para coluna_margem.\n");
        free(linha_codigo);
        free(linha_margem);
        return 1;
    }
    for (int i = 0; i < margem; i++) {
        coluna_margem[i] = '0';
    }


    // Imprimindo os códigos já expandidos e com as margens, com a altura informada
    for (int i = 0; i < altura; i++) {
        printf("%s", coluna_margem);    // Margem esquerda
        printf("%s", linha_codigo);     // Código de barras expandido
        printf("%s\n", coluna_margem);  // Margem direita
    }

    // Imprimindo margens inferiores
    for (int i = 0; i < margem; i++) {
        printf("%s\n", linha_margem);
    }
    printf("\n========================FIM DO ARQUIVO PBM========================\n");

    free(coluna_margem);
    free(linha_margem);
    free(linha_codigo);


    return 0;
}
