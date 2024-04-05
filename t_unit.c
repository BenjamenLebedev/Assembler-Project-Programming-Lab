
#include <stdio.h>
#include <ctype.h>
#include <errno.h> 
#include <string.h>
#include <stdlib.h> 
#include "t_unit.h" 
#include "vector.h"

struct translation_unit *create_translation_unit() {
    struct translation_unit *unit = (struct translation_unit *)malloc(sizeof(struct translation_unit));
    if (unit != NULL) {
        unit->code_image = NULL;
        unit->data_image = NULL;
        unit->IC = 0;
        unit->DC = 0;
        unit->symbols = new_vector(NULL, NULL);
        unit->externals = new_vector(NULL, NULL);
    }
    return unit;
}
  
void add_to_code_image(struct translation_unit *unit, int value) {
    if (unit == NULL) {
        printf("Error: Invalid translation unit.\n");
        return;
    }
    if (unit->IC % 10 +1 == 1) {    
        int *temp = (int *)realloc(unit->code_image, (unit->IC + 10 - 1) * sizeof(int));
        printf("relocating memory in add_to_code_image func\n");
        if (temp == NULL) {
            printf("Error: Memory reallocation failed for code_image.\n");
            return;
        }
        unit->code_image = temp;
    }
    unit->code_image[unit->IC] = value;
}

void add_to_data_image(struct translation_unit *unit, int value) {
    if (unit == NULL) {
        printf("Error: Invalid translation unit.\n");
        return;
    }
    if (unit->DC % 10 +1== 1) {
        int *temp = (int *)realloc(unit->data_image, (unit->DC + 10 - 1) * sizeof(int));
        if (temp == NULL) {
            printf("Error: Memory reallocation failed for data_image.\n");
            return;
        }
        unit->data_image = temp;
    }
    unit->data_image[unit->DC] = value;
}

void free_translation_unit(struct translation_unit *unit) {
    if (unit != NULL) {
        free(unit->code_image);
        free(unit->data_image);
        vector_destroy(&unit->symbols);
        vector_destroy(&unit->externals);
        free(unit);
    }
}

void printBinary14(int num) {
    int i;
    /* start from the most significant bit and print each bit */
    for (i = 13; i >= 0; i--) {
        /* check if the i-th bit is set or not */
        int bit = (num >> i) & 1;
        printf("%d", bit);
    }
    printf("\n");
}

void print_code_image(struct translation_unit *unit) {

    int i; 
    if (unit == NULL) {
        printf("Error: Invalid translation unit.\n");
        return;
    }

    /* check if the code_image array is NULL */
    if (unit->code_image == NULL) {
        printf("Error: Empty code image.\n");
        return;
    }

    /* Check if IC is zero */
    if (unit->IC == 0) {
        printf("Error: IC is zero.\n");
        return;
    }

    printf("Code Image:\n");
    for (i = 0; i < unit->IC; i++) {
        printBinary14(unit->code_image[i]);
        printf("%d\n", unit->code_image[i]);
    }
    printf("\n");
}


void print_data_image(struct translation_unit *unit) {

    int i; 
    if (unit == NULL) {
    printf("Error: Invalid translation unit.\n");
    return;
    }

    /* check if the code_image array is NULL */
    if (unit->data_image == NULL) {
        printf("Error: Empty code image.\n");
        return;
    }

    /* Check if IC is zero */
    if (unit->DC == 0) {
        printf("Error: IC is zero.\n");
        return;
    }

    printf("data Image:\n");
    for (i = 0; i < unit->DC; i++) {
        printBinary14(unit->data_image[i]);
        printf("%d\n", unit->data_image[i]);
    }
    printf("\n");
}