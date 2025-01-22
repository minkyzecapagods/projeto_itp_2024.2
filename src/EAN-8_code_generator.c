#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <definitions.h>
#include <funcs.h>
#include <io.h>

int main(const int argc, char *argv[]) {
        // 1. Receber um código de 8 dígitos e verificar se ele é válido;
        // 2. Se for válido, converter o código de 8 dígitos para código EAN-8;
        // 3. A partir do código EAN-8, criar uma imagem PBM


        //Ao chamar essa função sem nenhum argumento, exibe o uso no terminal
        if (argc == 1) {
                generator_usage();
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
                                if(num == 0 || num > MAX_SIZE) {
                                        fprintf(stderr, "ERRO: Valor inválido para opção '-h'.\n");
                                        return 1;
                                }
                                input.height = atoi(optarg);
                                break;
                        case 'm':
                                num = atoi(optarg);
                                if(num == 0 || num > MAX_SIZE) {
                                        fprintf(stderr, "ERRO: Valor inválido para opção '-m'.\n");
                                        return 1;
                                }
                                input.margin = atoi(optarg);
                                break;
                        case 'a':
                                num = atoi(optarg);
                                if(num == 0 || num > MAX_SIZE) {
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
        pbm_image.width = (CODE_LEN * input.area) + (input.margin * 2);
        pbm_image.code = to_ean8(input.identifier);
        sprintf(pbm_image.filename, "%s%s%s", "../barcode-output/", input.title, ".pbm");

        if (fopen(pbm_image.filename, "r") != NULL) {
                fprintf(stderr, "AVISO O arquivo '%s' já existe.\n"
                                "Deseja sobrescrever a pasta existente? (s/n) ", pbm_image.filename);
                char r;
                while (1) {
                        r = getchar();
                        if (r == 's' || r == 'S') break;
                        if (r == 'n' || r == 'N') {
                                printf("Operação cancelada. O arquivo não será sobrescrito.\n");
                                return 0;
                        }
                        printf("Entrada inválida. Por favor, digite 's' para sobrescrever ou 'n' para cancelar: ");
                        while ((r = getchar()) != '\n' && r != EOF);
                }
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

        if (pbm_image.height > MAX_SIZE || pbm_image.width > MAX_SIZE) {
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