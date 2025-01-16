#include <stdio.h>
#include <string.h>
#include <math.h>

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

void traduzirEAN8(int identificador) {
    char lcodes[10][8] = {
        "0001101", "0011001", "0010011", "0111101", "0100011",
        "0110001", "0101111", "0111011", "0110111", "0001011"
    };
    char rcodes[10][8] = {
        "1110010", "1100110", "1101100", "1000010", "1011100",
        "1001110", "1010000", "1000100", "1001000", "1110100"
    };

    // Código de início do EAN-8
    char codigo[100] = "101";

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
    printf("Código de Barras EAN-8: %s\n", codigo);
}

int main() {
    int identificador;
    printf("Digite um identificador de 8 dígitos: ");
    scanf("%d", &identificador);

    int digitoVerificadorEsperado = calcularDigitoVerificador(identificador);
    int digitoVerificadorAtual = identificador % 10;

    if (digitoVerificadorEsperado == digitoVerificadorAtual) {
        printf("Identificador válido.\n");
        traduzirEAN8(identificador);
    } else {
        printf("Erro: Dígito verificador inválido.\n"
               "Para o código que você inseriu, o último digito deve ser: %d\n\n", digitoVerificadorEsperado);
        printf("Nova tentativa necessária.\n");
    }

    return 0;
}
