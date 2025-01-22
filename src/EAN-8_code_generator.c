#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void usage(){
        printf("GERADOR DE CÓDIGO DE BARRAS\n");
        printf("\tGera um arquivo PBM na pasta 'barcode-output' com base na entrada fornecida.\n");
        printf("Uso:\n");
        printf("\t./EAN-8_code_generator <opção> ... <identificador>\n");
        printf("Opções:\n");
        printf("\t-m <pixels>\n");
        printf("\t\tPermite ao usuário definir a margem de acordo com o valor inteiro fornecido em <pixels>\n");
        printf("\t\tSem o parâmetro -m, a margem será de 4px.\n");
        printf("\t-a <pixels>\n");
        printf("\t\tPermite ao usuário definir a área do código de barras de acordo com o valor inteiro fornecido em <pixels>\n");
        printf("\t\tSem o parâmetro -a, a área será de 3px.\n");
        printf("\t-h <pixels>\n");
        printf("\t\tPermite ao usuário definir a altura do código de barras de acordo com o valor inteiro fornecido em <pixels>\n");
        printf("\t\tSem o parâmetro -h, a altura será de 50px.\n");
        printf("\t-n <nome_arquivo>\n");
        printf("\t\tPermite ao usuário definir o nome do arquivo de saída de acordo com o valor fornecido em <nome_arquivo>\n");
        printf("\t\tSem o parâmetro -n, o nome será o identificador.\n");
}

typedef struct {
        int width;
        int height;
        char filename[43];
        char *code;
} PBMImage;

typedef struct {
        int margin;
        int area;
        int height;
        char identifier[9];
        char title[21];
} GenInfo;

// Função para calcular o dígito verificador

char get_verification_digit(const char *id) {
        int sum = 0;
        for (int i = 1; i < 8; i++) {
                sum += (id[i-1] - '0') * ((i % 2 == 1) ? 3 : 1);
        }
        int next_mul_ten = ((sum + 9) / 10) * 10; // Encontra o próximo múltiplo de 10
        int digit = next_mul_ten - sum;
        return digit + '0';
}

char* to_ean8(const char *id) {
        const char l_codes[10][8] = {
                "0001101", "0011001", "0010011", "0111101", "0100011",
                "0110001", "0101111", "0111011", "0110111", "0001011"
        };
        const char r_codes[10][8] = {
                "1110010", "1100110", "1101100", "1000010", "1011100",
                "1001110", "1010000", "1000100", "1001000", "1110100"
        };

        // Código de início do EAN-8
        static char code_line[68] = "101";
        int digit;

        // Processa os primeiros 4 dígitos com L-code
        for (int i = 0; i < 4; i++) {
                digit = id[i] - '0';
                strcat(code_line, l_codes[digit]);
        }

        // Adiciona o código do meio
        strcat(code_line, "01010");

        // Processa os últimos 4 dígitos com R-code
        for (int i = 4; i < 8; i++) {
                digit = id[i] - '0';
                strcat(code_line, r_codes[digit]);
        }

        // Adiciona o código de fim
        strcat(code_line, "101");

        // Imprime o código de barras traduzido
        return code_line;
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
                fprintf(stderr, "ERRO DE ENTRADA: Muitos argumentos fornecidos. Verifique a quantidade correta de opções.\n");
                return 1;
        }
        GenInfo input = {4, 3, 50, 'e'};
        int opt, num;
        //Lida com os argumentos parseados
        //OBS: preciso trocar o atoi por strtol e considerar reportar erros

        while ((opt = getopt(argc, argv, ":m:a:h:n:")) != -1) {
                switch(opt){
                        case 'h':
                                num = atoi(optarg);
                                if(num == 0 || num > 1024) {
                                        fprintf(stderr, "ERRO: Valor inválido para opção '-h'.\n");
                                        return 1;
                                }
                                input.height = atoi(optarg);
                                break;
                        case 'm':
                                num = atoi(optarg);
                                if(num == 0 || num > 1024) {
                                        fprintf(stderr, "ERRO: Valor inválido para opção '-m'.\n");
                                        return 1;
                                }
                                input.margin = atoi(optarg);
                                break;
                        case 'a':
                                num = atoi(optarg);
                                if(num == 0 || num > 1024) {
                                        fprintf(stderr, "ERRO: Valor inválido para opção '-a'.\n");
                                        return 1;
                                }
                                input.area = atoi(optarg);
                                break;
                        case 'n':
                                if (strlen(optarg) > 20) {
                                        fprintf(stderr, "ERRO DE ENTRADA: "
                                                        "O nome do arquivo deve ter no máximo 20 caracteres.\n");
                                        return 1;
                                }
                                if(optarg != 0) strcpy(input.title, optarg);
                                break;
                        case ':':
                                fprintf(stderr, "ERRO DE EMTRADA: A opção '-%c' requer um valor.\n", optopt);
                                return 1;
                        case '?':
                                fprintf(stderr,"ERRO DE ENTRADA: '-%c' é uma opção inválida.\n", optopt);
                                return 1;
                        default:
                                fprintf(stderr, "ERRO: Erro inesperado.\n");
                                return 1;
                }
        }
        //Verifica se o código de barra está sendo parseado
        if (optind < argc) {
                for(int i = optind; i < argc; i++) {
                        if (strlen(argv[i]) != 8) {
                                fprintf(stderr, "ERRO DE ENTRADA: O identificador deve conter exatamente 8 caracteres.\n");
                                return 1;
                        }
                        if (input.title[0] == '\0') {
                                strcpy(input.title, argv[i]);
                        }
                        int num_identifier = atoi(argv[i]);
                        if (num_identifier > 0) {
                                if (input.identifier[0] == 'e') {
                                        for (int j = 7; j >= 0; j--) {
                                                char id = (num_identifier % 10) + '0';
                                                input.identifier[j] = id;
                                                num_identifier /= 10;
                                        }
                                } else {
                                        fprintf(stderr, "ERRO DE ENTRADA: Mais de um identificador encontrado.\n");
                                        return 1;
                                }
                        }
                        else {
                                fprintf(stderr, "ERRO DE ENTRADA: Identificador inválido.\n");
                                return 1;
                        }
                }
        } else {
                fprintf(stderr, "ERRO DE ENTRADA: Identificador não encontrado.\n");
                return 1;
        }

        input.identifier[8] = '\0';

        PBMImage pbm_image;
        pbm_image.height  = input.height + (input.margin * 2);
        pbm_image.width = (67 * input.area) + (input.margin * 2);
        pbm_image.code = to_ean8(input.identifier);
        sprintf(pbm_image.filename, "%s%s%s", "../barcode-output/", input.title, ".pbm");

        if (fopen(pbm_image.filename, "r") != NULL) {
                fprintf(stderr, "ERRO DE ENTRADA: O arquivo '%s'.pbm já existe.\n", pbm_image.filename);
                return 1;
        }
        FILE *output_file = fopen(pbm_image.filename, "w");
        if (output_file == NULL) {
                fprintf(stderr, "ERRO: Algo deu errado ao abrir o arquivo.\n");
                return 1;
        }

        const char verifier = get_verification_digit(input.identifier);
        if (input.identifier[7] != verifier) {
                fprintf(stderr, "ERRO DE ENTRADA: Dígito verificador inválido. O último dígito deve ser: %c.\n", verifier);
                return 1;
        }

        if (pbm_image.height > 1024 || pbm_image.width > 1024) {
                fprintf(stderr, "ERRO DE ENTRADA: As dimensões da imagem excedem o limite permitido.\n");
                return 1;
        }

        // barcode_line é a codificação das barras expandida baseada no parâmetro área
        char* barcode_line = malloc(sizeof(char) * (pbm_image.width + 1));
        if (barcode_line == NULL) {
                fprintf(stderr, "ERRO: Falha na alocação de memória.\n");
                return 1;
        }

        // Expandindo o código de barras conforme a área
        for (int i = 0; i < strlen(pbm_image.code); i++) {
                for (int j = 0; j < input.area; j++) {
                        barcode_line[i * input.area + j] = pbm_image.code[i];
                }
        }
        barcode_line[pbm_image.width] = '\0';

        // margin_line é a linha que servirá como margem superior e inferior no arquivo PBM
        char* margin_line = malloc(sizeof(char) * (pbm_image.width + 1));
        if (margin_line == NULL) {
                fprintf(stderr, "ERRO: Falha na alocação de memória.\n");
                free(barcode_line);
                return 1;
        }
        for (int i = 0; i < pbm_image.width; i++) {
                margin_line[i] = '0';
        }
        margin_line[pbm_image.width] = '\0';

        // coluna_margem representa as margens laterais do arquivo PBM
        char* coluna_margem = malloc(sizeof(char) * (input.margin + 1));
        if (coluna_margem == NULL) {
                fprintf(stderr, "ERRO: Falha na alocação de memória.\n");
                free(barcode_line);
                free(margin_line);
                return 1;
        }
        for (int i = 0; i < input.margin; i++) {
                coluna_margem[i] = '0';
        }

        fprintf(output_file, "P1\n");
        fprintf(output_file, "%d %d\n", pbm_image.width, pbm_image.height);

        // Imprimindo margens superiores
        for (int i = 0; i < input.margin; i++) fprintf(output_file, "%s\n", margin_line);

        // Imprimindo os códigos já expandidos e com as margens, com a altura informada
        for (int i = 0; i < input.height; i++) {
                fprintf(output_file, "%s", coluna_margem);
                fprintf(output_file, "%s", barcode_line);
                fprintf(output_file,"%s\n", coluna_margem);
        }

        // Imprimindo margens inferiores
        for (int i = 0; i < input.margin; i++) fprintf(output_file,"%s\n", margin_line);

        fclose(output_file);
        free(coluna_margem);
        free(barcode_line);
        free(margin_line);

        return 0;
}