
#include "../Header_Files/global_var.h"
#include "../Header_Files/t_unit.h" 
#include "../Header_Files/vector_lib.h"
 
struct translation_unit *create_translation_unit() {
    /*allocate memory for the translation unit*/
    struct translation_unit *unit = (struct translation_unit *)malloc(sizeof(struct translation_unit));
    if(unit == NULL) {
        printf("********** error: Memory allocation failed for translation unit.\n");
        return NULL;
    }
    if (unit != NULL) { /*check if memory allocation succeeded*/
        /*initialize values*/
        unit->IC = 0; 
        unit->DC = 0;
        unit->code_image = NULL;
        unit->data_image = NULL;
        unit->symbols = new_vector(symbol_ctor, symbol_dtor);
        unit->externals = new_vector(extern_ctor, extern_dtor);
        unit->extern_use = FALSE;
        unit->entry_use = FALSE;
    }
    return unit;
}


  
void add_to_code_image(struct translation_unit *unit, int value) {
    if (unit == NULL) {
        printf("********** error: Invalid translation unit.\n");
        return;
    }
    /*dynamic allocation of memory*/
    if ((unit->IC % 10 +1) == 1) { /*if we to the 10th cell, increase the size by 10*/
        unit->code_image = (int *)realloc(unit->code_image, (unit->IC + 10) * sizeof(int));
        if (unit->code_image == NULL) {
            printf("********** error: Memory reallocation failed for code_image.\n");
            free_translation_unit(unit);
            exit(1);
        }
    }
    unit->code_image[unit->IC] = value;
}

void add_to_data_image(struct translation_unit *unit, int value) {
    if (unit == NULL) {
        printf("********** error: Invalid translation unit.\n");
        return;
    }
    /*dynamic allocation of memory*/
    if ((unit->DC % 10 +1)== 1) { /*if we to the 10th cell, increase the size by 10*/
        unit->data_image = (int *)realloc(unit->data_image, (unit->DC + 10) * sizeof(int));
        if (unit->data_image == NULL) {
            printf("********** error: Memory reallocation failed for data_image.\n");
            return;
        }
    }
    unit->data_image[unit->DC] = value;
}

void free_translation_unit(struct translation_unit *unit) {
    if (unit != NULL) {
        /*free everything*/
        free(unit->code_image);
        free(unit->data_image);
        vector_destroy(&unit->symbols);
        vector_destroy(&unit->externals);
        free(unit);
    }
}

/*prints to stdout the binary form of integer num
with 14 bit*/
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
        printf("********** error: Invalid translation unit.\n");
        return;
    }

    /* check if the code_image array is NULL */
    if (unit->code_image == NULL) {
        printf("********** error: Empty code image.\n");
        return;
    }

    /* Check if IC is zero */
    if (unit->IC == 0) {
        printf("********** error: IC is zero.\n");
        return;
    }

    printf("Code Image:\n");
    for (i = 0; i < unit->IC; i++) {
        printf("%d  ",100+i);
        printBinary14(unit->code_image[i]);
        printf("       %d\n", unit->code_image[i]);
 
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