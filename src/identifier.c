//
// Created by tuzin on 1/7/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *int_to_str(int n) {
        // Aloca memória para armazenar a string
        char *str = (char *)malloc(12 * sizeof(char));  // 12 é suficiente para armazenar até um inteiro de 32 bits
        if (str == NULL) {
                printf("Erro de alocação de memória\n");
                return NULL;
        }
        // Converte o número em string
        sprintf(str, "%d", n);
        return str;
}


int main() {
        int lcodes[10][1] = {
                {0001101}, {0011001}, {0010011}, {0111101}, {0100011},
                {0110001}, {0101111}, {0111011}, {0110111}, {0001011}
        };
        int rcodes[10][1] = {
                {1110010}, {1100110}, {1101100}, {1000010}, {1011100},
                {1001110}, {1010000}, {1000100}, {1001000}, {1110100}
        };
        int identifier;

        char barcode[68] = "101";

        scanf("%d", &identifier);
        char *str = int_to_str(identifier);
        for (int i = 0; i < 4; i++) {
                strcat(barcode, int_to_str(lcodes[atoi(str[i])][0]));
        }
        for (int i = 4; i < 8; i++) {
                strcat(barcode, int_to_str(rcodes[atoi(str[i])][0]));
        }
        printf("%s\n", barcode);
        free(str);



        return 0;
}
