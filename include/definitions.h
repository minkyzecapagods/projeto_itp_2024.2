//
// Created by zecapagods on 1/4/25.
//

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

// Cons
extern const char r_codes[10][8];
extern const char l_codes[10][8];

// Defines
#define MAX_SIZE 1024
#define CODE_LEN 67

// Structs
typedef struct {
        int margin;
        int area;
        int height;
        char identifier[9];
        char title[21];
} GenInfo;

typedef struct {
        int width;
        int height;
        char filename[41];
        char *ean8_code;
        char *barcode_line;
} PBMImage;

#endif //DEFINITIONS_H
