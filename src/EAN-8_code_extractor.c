//
// Created by tuzin on 12/31/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void usage(){
    printf("BARCODE READER\n");
    printf("\tExtracts the barcode indentifier from the input PBM file.\n");
    printf("Usage:\n");
    printf("\t/EAN-8_code_extractor <input_file>\n");
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    usage();
    exit(-1);
  }
  char *input_file = argv[1];
  FILE *input = fopen(input_file, "r");
  // if (input == NULL) {
  // Por algum motivo NULL provoca um erro de unknown identifier
  if (input == 0) {
    printf("Error opening input file\n");
    printf("%s\n", strerror(errno));
  }
  return 0;
}