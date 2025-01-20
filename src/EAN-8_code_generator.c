#include <stdbool.h>
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
        int **pixels;
} PBMImage;

typedef struct {
        int margin;
        int area;
        int height;
        int identifier[8];
        char title[21];
} INPUTInfo;

int main(int argc, char *argv[]) {
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
        int opt;
        //Lida com os argumentos parseados
        //OBS: preciso trocar o atoi por strtol e considerar reportar erros

        while ((opt = getopt(argc, argv, ":m:a:h:n:")) != -1) {
                switch(opt){
                        case 'h':
                                if(atoi(optarg) == 0) {
                                        fprintf(stderr, "ERROR: Invalid value in option '-h'.\n");
                                        return 1;
                                }
                                input.height = atoi(optarg);
                                break;
                        case 'm':
                                if(atoi(optarg) == 0) {
                                        fprintf(stderr, "ERROR: Invalid value in option '-m'.\n");
                                        return 1;
                                }
                                input.margin = atoi(optarg);
                                break;
                        case 'a':
                                if(atoi(optarg) == 0) {
                                        fprintf(stderr, "ERROR: Invalid value in option '-a'.\n");
                                        return 1;
                                }
                                input.area = atoi(optarg);
                                break;
                        case 'n':
                                if(atoi(optarg) == 0) {
                                        fprintf(stderr, "ERROR: Invalid value in option '-n'.\n");
                                        return 1;
                                }
                                if (strlen(optarg) > 20) {
                                        fprintf(stderr, "INPUT ERROR: Name is too long.\n");
                                        return 1;
                                }
                                if(optarg != 0) strcpy(input.title, optarg);
                                break;
                        case ':':
                                fprintf(stderr, "INPUT ERROR: Option '-%c' needs a value.\n", optopt);
                                return 1;
                        break;
                        case '?':
                                fprintf(stderr,"INPUT ERROR: '-%c' is an unknown option.\n", optopt);
                                return 1;
                        break;
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
                                        for (int i = 7; i >= 0; i--) {
                                                input.identifier[i] = num % 10;
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

        printf("Margin is %d\n", input.margin);
        printf("Area is %d\n", input.area);
        printf("Height is %d\n", input.height);
        printf("Name is %s\n", input.title);
        for (int i = 0; i < 8; i++) {
                printf("%d", input.identifier[i]);
        }
        return 0;
}
