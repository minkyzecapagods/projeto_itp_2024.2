//
// Created by zecapagods on 1/22/25.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/io.h"
#include "../include/definitions.h"

void generator_usage() {
        printf("\tGera um arquivo PBM baseado no input do usuário.\n");
        printf("Uso:\n");
        printf("\t./EAN-8_code_generator <opção> ... <identificador>\n");
        printf("Ooções:\n");
        printf("\t-m <pixels>\n");
        printf("\t\tdeixa o usuário definir a margem baseado no input em <pixels>\n");
        printf("\t\tsem -m, a margem será de 4px.\n");
        printf("\t-a <pixels>\n");
        printf("\t\tdeixa o usuário definir a área das barras baseada no input em <pixels>\n");
        printf("\t\tsem -a, a área será de 3px.\n");
        printf("\t-h <pixels>\n");
        printf("\t\tdeixa o usuário definir a altura do código de barras baseada no input em <pixels>\n");
        printf("\t\tserá -h, a altura será de 50px.\n");
        printf("\t-n <file_name>\n");
        printf("\t\tdeixa o usuário definir o nome do arquivo .pbm gerado baseado no input <file_name>\n");
        printf("\t\tsem -n, o nome do arquivo será o identificador fornecido.\n");
}

void extractor_usage() {
        printf("LEITOR DE CÓDIGO DE BARRAS\n");
        printf("\tExtrai o identificador do código de barras no arquivo .pbm informado.\n");
        printf(
                "\tPara acessar arquivos dentro da pasta 'barcode-output', inserir o caminho '../barcode-output/' antes do nome do arquivo.\n");
        printf("Uso:\n");
        printf("\t./EAN-8_code_extractor <nome_do_arquivo>\n");
}

void check_size(int width, int height) {
        if (height > MAX_SIZE || width > MAX_SIZE) {
                fprintf(stderr, "ERRO DE ENTRADA: As dimensões da imagem excedem o limite permitido.\n");
                exit(1);
        }
}

void check_file_exists(char *filename) {
        if (fopen(filename, "r") != NULL) {
                fprintf(stderr, "AVISO O arquivo '%s' já existe.\n"
                        "Deseja sobrescrever a pasta existente? (s/n) ", filename);
                char r;
                while (1) {
                        r = getchar();
                        if (r == 's' || r == 'S') break;
                        if (r == 'n' || r == 'N') {
                                printf("Operação cancelada. O arquivo não será sobrescrito.\n");
                                exit(1);
                        }
                        printf("Entrada inválida. Por favor, digite 's' para sobrescrever ou 'n' para cancelar: ");
                        while ((r = getchar()) != '\n' && r != EOF);
                }
        }
}

int check_flags(char *optarg, char *flag) {
        char *endptr;
        int num = (int) strtol(optarg, &endptr, 10);
        if (num < 0 || num > MAX_SIZE || *endptr == '\0') {
                fprintf(stderr, "ERRO: Valor inválido para opção '%s'.\n", flag);
                exit(1);
        }
        return num;
}

// Função para checar os possíveis erros que podem existir no arquivo pbm.
int check_barcode_file(FILE *filename) {
        char first_line[3];
        char st1[5];
        char st2[5];
        fscanf(filename, "%s", first_line);
        if (strcmp(first_line, "P1") != 0) {
                fprintf(stderr, "ERRO: Tipo de arquivo inválido.\n"
                        "O arquivo não é do tipo P1.\n");
                goto cleanup;
        }
        if (fscanf(filename, "%s %s", st1, st2) == EOF) {
                fprintf(stderr, "ERRO: Tipo de arquivo inválido.\n"
                        "Não foi possível ler as dimensões do código de barras.\n");
                goto cleanup;
        };
        int width = atoi(st1);
        if (width <= 0 || width > MAX_SIZE) {
                fprintf(stderr, "ERRO: Tipo de arquivo inválido.\n"
                        "O arquivo é muito grande.\n");
                goto cleanup;
        }
        return width;

cleanup:
        if (filename) if (fclose(filename) == EOF) fprintf(stderr, "ERRO: Erro crítico ao fechar o arquivo.\n");
        exit(1);
}

// Função para criar o arquivo pbm.
void create_pbm_image(PBMImage pbm_image, int height, int margin) {
        // margin_line é a linha que servirá como margem superior e inferior no arquivo PBM
        char *margin_line = malloc(sizeof(char) * (pbm_image.width + 1));
        if (margin_line == NULL) {
                fprintf(stderr, "ERRO: Falha na alocação de memória.\n");
                free(pbm_image.barcode_line);
                exit(1);
        }
        for (int i = 0; i < pbm_image.width; i++) margin_line[i] = '0';
        margin_line[pbm_image.width] = '\0';

        // margin_column representa as margens laterais do arquivo PBM
        char *margin_column = malloc(sizeof(char) * (margin + 1));
        if (margin_column == NULL) {
                fprintf(stderr, "ERRO: Falha na alocação de memória.\n");
                free(pbm_image.barcode_line);
                free(margin_line);
                exit(1);
        }
        for (int i = 0; i < margin; i++) margin_column[i] = '0';


        FILE *output_file = fopen(pbm_image.filename, "w");
        if (output_file == NULL) {
                fprintf(stderr, "ERRO: Algo deu errado ao abrir o arquivo.\n");
                free(pbm_image.barcode_line);
                free(margin_line);
                free(margin_column);
                exit(1);
        }

        fprintf(output_file, "P1\n");
        fprintf(output_file, "%d %d\n", pbm_image.width, pbm_image.height);

        // Imprimindo margens superiores
        for (int i = 0; i < margin; i++) fprintf(output_file, "%s\n", margin_line);

        // Imprimindo os códigos já expandidos e com as margens, com a altura informada
        for (int i = 0; i < height; i++) {
                fprintf(output_file, "%s", margin_column);
                fprintf(output_file, "%s", pbm_image.barcode_line);
                fprintf(output_file, "%s\n", margin_column);
        }

        // Imprimindo margens inferiores
        for (int i = 0; i < margin; i++) fprintf(output_file, "%s\n", margin_line);

        fclose(output_file);
        free(margin_column);
        free(pbm_image.barcode_line);
        free(margin_line);
}
