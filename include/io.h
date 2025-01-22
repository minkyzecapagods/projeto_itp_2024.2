//
// Created by zecapagods on 1/22/25.
//

#ifndef IO_H
#define IO_H

#include "definitions.h"

void extractor_usage();

void generator_usage();

void check_size(int width, int height);

void check_file_exists(char* filename);

int check_barcode_file(FILE* filename);

void create_pbm_image(PBMImage pbm_image, int height, int margin);

#endif //IO_H
