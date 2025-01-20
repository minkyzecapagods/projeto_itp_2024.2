#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void usage(){
        printf("BARCODE GENERATOR\n");
        printf("\tGenerates a PBM file based on the input.\n");
        printf("Usage:\n");
        printf("\t./EAN-8_code_generator <option> ... <identifier>\n");
        printf("Options:\n");
        printf("\t-m <pixels>\n");
        printf("\t\tlet the user define the margin according to the integral input in <pixels>\n");
        printf("\t\twithout -m, the margin will be 4px\n");
        printf("\t-a <pixels>\n");
        printf("\t\tlet the user define the area for the code according to the integral input in <pixels>\n");
        printf("\t\twithout -a, the area will be 3px\n");
        printf("\t-h <pixels>\n");
        printf("\t\tlet the user define the height for the barcode according to the integral input in <pixels>\n");
        printf("\t\twithout -h, the height will be 50px\n");
        printf("\t-n <file_name>\n");
        printf("\t\tlet the user define the name for the output file according to the input in <file_name>\n");
        printf("\t\twithout -n, the name will be the identifier\n");
}

typedef struct {
        int width;
        int height;
        char title[28];
        char *code;
} PBMImage;

typedef struct {
        int margin;
        int area;
        int height;
        int identifier[8];
        char title[21];
} INPUTInfo;

// Função para calcular o dígito verificador

int calcularDigitoVerificador(const int *id) {
        int soma = 0;
        for (int i = 1; i < 8; i++) {
                soma += id[i-1] * ((i % 2 == 1) ? 3 : 1);
        }
        int proximoMultiploDez = ((soma + 9) / 10) * 10; // Encontra o próximo múltiplo de 10
        return proximoMultiploDez - soma;
}

char* traduzirEAN8(const int *id) {
        const char lcodes[10][8] = {
                "0001101", "0011001", "0010011", "0111101", "0100011",
                "0110001", "0101111", "0111011", "0110111", "0001011"
        };
        const char rcodes[10][8] = {
                "1110010", "1100110", "1101100", "1000010", "1011100",
                "1001110", "1010000", "1000100", "1001000", "1110100"
        };

        // Código de início do EAN-8
        static char codigo[100] = "101";

        // Processa os primeiros 4 dígitos com L-code
        for (int i = 0; i < 4; i++) {
                strcat(codigo, lcodes[id[i]]);
        }

        // Adiciona o código do meio
        strcat(codigo, "01010");

        // Processa os últimos 4 dígitos com R-code
        for (int i = 4; i < 8; i++) {
                strcat(codigo, rcodes[id[i]]);
        }

        // Adiciona o código de fim
        strcat(codigo, "101");

        // Imprime o código de barras traduzido
        return codigo;
}

int main(const int argc, char *argv[]) {
        // 1. Receber um código de 8 dígitos e verificar se ele é válido;
        // 2. Se for válido, converter o código de 8 dígitos para código EAN-8;
        // 3. A partir do código EAN-8, criar uma imagem PBM


        //Ao chamar essa função sem nenhum argumento, exibe o uso no terminal
        if (argc == 1) {
                usage();
                return 1;
        }
        if (argc > 10) {
                fprintf(stderr, "INPUT ERROR: Too many arguments.\n");
                return 1;
        }
        INPUTInfo input = {4, 3, 50, -1};
        int opt, num;
        //Lida com os argumentos parseados
        //OBS: preciso trocar o atoi por strtol e considerar reportar erros

        while ((opt = getopt(argc, argv, ":m:a:h:n:")) != -1) {
                switch(opt){
                        case 'h':
                                num = atoi(optarg);
                                if(num == 0 || num > 1000) {
                                        fprintf(stderr, "ERROR: Invalid value in option '-h'.\n");
                                        return 1;
                                }
                                input.height = atoi(optarg);
                                break;
                        case 'm':
                                num = atoi(optarg);
                                if(num == 0 || num > 1000) {
                                        fprintf(stderr, "ERROR: Invalid value in option '-m'.\n");
                                        return 1;
                                }
                                input.margin = atoi(optarg);
                                break;
                        case 'a':
                                num = atoi(optarg);
                                if(num == 0 || num > 1000) {
                                        fprintf(stderr, "ERROR: Invalid value in option '-a'.\n");
                                        return 1;
                                }
                                input.area = atoi(optarg);
                                break;
                        case 'n':
                                if (strlen(optarg) > 20) {
                                        fprintf(stderr, "INPUT ERROR: Name is too long.\n");
                                        return 1;
                                }
                                if(optarg != 0) strcpy(input.title, optarg);
                                break;
                        case ':':
                                fprintf(stderr, "INPUT ERROR: Option '-%c' needs a value.\n", optopt);
                                return 1;
                        case '?':
                                fprintf(stderr,"INPUT ERROR: '-%c' is an unknown option.\n", optopt);
                                return 1;
                        default:
                                fprintf(stderr, "ERROR: Unexpected error.\n");
                                return 1;
                }
        }
        //Verifica se o código de barra está sendo parseado
        if (optind < argc) {
                for(int i = optind; i < argc; i++) {
                        if (strlen(argv[i]) != 8) {
                                fprintf(stderr, "INPUT ERROR: Identifier must be 8 characters long.\n");
                                return 1;
                        }
                        if (input.title[0] == '\0') {
                                strcpy(input.title, argv[i]);
                        }
                        int num = atoi(argv[i]);
                        if (num > 0) {
                                if (input.identifier[0] == -1) {
                                        for (int j = 7; j >= 0; j--) {
                                                input.identifier[j] = num % 10;
                                                num /= 10;
                                        }
                                } else {
                                        fprintf(stderr, "INPUT ERROR: More than one identifier.\n");
                                        return 1;
                                }
                        }
                        else {
                                fprintf(stderr, "INPUT ERROR: Invalid identifier");
                                return 1;
                        }
                }
        } else {
                fprintf(stderr, "INPUT ERROR: Missing barcode identifier.\n");
                return 1;
        }

        const int digitoVerificadorEsperado = calcularDigitoVerificador(input.identifier);
        if (input.identifier[7] != digitoVerificadorEsperado) {
                fprintf(stderr, "INPUT ERROR: Dígito verificador inválido.\n"
               "Para o código que você inseriu, o último digito deve ser: %d\n", digitoVerificadorEsperado);
                return 1;
        }

        PBMImage pbmImage;
        pbmImage.height  = input.height + (input.margin * 2);
        pbmImage.width = (67 * input.area) + (input.margin * 2);
        pbmImage.code = traduzirEAN8(input.identifier);
        char *pre = "../";
        char *pbm = ".pbm";
        sprintf(pbmImage.title, "%s%s%s", pre, input.title, pbm);

        if (pbmImage.height > 1000 || pbmImage.width > 1000) {
                fprintf(stderr, "INPUT ERROR: Image too big.\n");
                return 1;
        }

        if (fopen(pbmImage.title, "r") != NULL) {
                fprintf(stderr, "INPUT ERROR: '%s' already exists.\n", pbmImage.title);
                return 1;
        }

        // linha_codigo é a codificação das barras expandida baseada no parâmetro área
        char* linha_codigo = malloc(sizeof(char) * (pbmImage.width + 1));
        if (linha_codigo == NULL) {
                fprintf(stderr, "ERROR: Falha na alocação de memória para linha_codigo.\n");
                return 1;
        }

        // Expandindo o código de barras conforme a área
        for (int i = 0; i < strlen(pbmImage.code); i++) {
                for (int j = 0; j < input.area; j++) {
                        linha_codigo[i * input.area + j] = pbmImage.code[i];
                }
        }
        linha_codigo[pbmImage.width] = '\0';

        // linha_margem é a linha que servirá como margem superior e inferior no arquivo PBM
        char* linha_margem = malloc(sizeof(char) * (pbmImage.width + 1));
        if (linha_margem == NULL) {
                fprintf(stderr, "Erro: Falha na alocação de memória para linha_margem.\n");
                free(linha_codigo);
                return 1;
        }
        for (int i = 0; i < pbmImage.width; i++) {
                linha_margem[i] = '0';
        }
        linha_margem[pbmImage.width] = '\0';

        // coluna_margem representa as margens laterais do arquivo PBM
        char* coluna_margem = malloc(sizeof(char) * (input.margin + 1));
        if (coluna_margem == NULL) {
                fprintf(stderr, "Erro: Falha na alocação de memória para coluna_margem.\n");
                free(linha_codigo);
                free(linha_margem);
                return 1;
        }
        for (int i = 0; i < input.margin; i++) {
                coluna_margem[i] = '0';
        }

        FILE *imagem = fopen(pbmImage.title, "w");

        printf("\n======================INICIO DO ARQUIVO PBM======================\n");
        printf("P1\n");
        fprintf(imagem, "P1\n");
        printf("%d %d\n", pbmImage.width, pbmImage.height);
        fprintf(imagem, "%d %d\n", pbmImage.width, pbmImage.height);

        // Imprimindo margens superiores
        for (int i = 0; i < input.margin; i++) {
                printf("%s\n", linha_margem);
                fprintf(imagem, "%s\n", linha_margem);
        }

        // Imprimindo os códigos já expandidos e com as margens, com a altura informada
        for (int i = 0; i < input.height; i++) {
                printf("%s", coluna_margem);    // Margem esquerda
                fprintf(imagem, "%s", coluna_margem);
                printf("%s", linha_codigo);     // Código de barras expandido
                fprintf(imagem, "%s", linha_codigo);
                printf("%s\n", coluna_margem);  // Margem direita
                fprintf(imagem,"%s\n", coluna_margem);
        }

        // Imprimindo margens inferiores
        for (int i = 0; i < input.margin; i++) {
                printf("%s\n", linha_margem);
                fprintf(imagem,"%s\n", linha_margem);
        }
        printf("\n========================FIM DO ARQUIVO PBM========================\n");

        printf("Margin is %d\n", input.margin);
        printf("Area is %d\n", input.area);
        printf("Height is %d\n", input.height);
        printf("Name is %s\n", input.title);
        for (int i = 0; i < 8; i++) {
                printf("%d", input.identifier[i]);
        }

        fclose(imagem);
        free(coluna_margem);
        free(linha_codigo);
        free(linha_margem);

        return 0;
}
