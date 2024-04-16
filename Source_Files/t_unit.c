
#include "../Header_Files/global_var.h"
#include "../Header_Files/t_unit.h" 
#include "../Header_Files/vector.h"

struct translation_unit *create_translation_unit() {
    struct translation_unit *unit = (struct translation_unit *)malloc(sizeof(struct translation_unit));
    if (unit != NULL) {
        unit->code_image = NULL;
        unit->data_image = NULL;
        unit->IC = 0; 
        unit->DC = 0;
        unit->symbols = new_vector(NULL, NULL);
        unit->externals = new_vector(NULL, NULL);
        unit->extern_use = FALSE;
        unit->entry_use = FALSE;
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
    int i,bit;
    /* start from the most significant bit and print each bit */
    for (i = WORD_LEN - 1; i >= 0; i--) {
        /* check if the i-th bit is set or not */
        bit = (num >> i) & 1;
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

/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/*temp debug functions */


int makeBinaryFile(const struct translation_unit *unit, char *FileName) {
    int i,total,is_error = FALSE;
    FILE *file;
    char *file_name,*binary_code;
    char *extension = ".binss";
    file_name = (char *)malloc(strlen(FileName) + strlen(extension) + 1);
    strcpy(file_name, FileName);
    strcat(file_name, extension);
    file = fopen(file_name, "w+");
    if (file == NULL) {
        printf("Error: Failed to create binary file.\n");
        return TRUE;
    }

    fprintf(file,"\n");
    for (total = i = 0; i < unit->IC; i++,total++) {
        binary_code = binary_seq(unit->code_image[i]);
        fprintf(file,"  %d  %s  (%d)\n", total, binary_code,unit->code_image[i]);
    }
    for (i = 0; i < unit->DC; i++,total++) {
        binary_code = binary_seq(unit->data_image[i]);
        fprintf(file,"  %d  %s  (%d)\n", total, binary_code,unit->data_image[i]);
    }

    fclose(file);
    free(file_name);
    return is_error;
}

char *binary_seq(int num) {
    char *binary; 
    int i;


    binary = (char *) malloc(15 * sizeof(char));
    if(binary == NULL){
        printf("Error: Memory allocation failed for binary_seq.\n");
        return NULL;
    }

    /*initializing all to zeros*/
    for (i = 0; i < WORD_LEN; i++) {
        binary[i] = '0';
    }

    /*converting the number to binary*/
    for (i = WORD_LEN - 1; i >= 0; i--) {
        binary[i] = ((num >> i) & 1) + '0';
    }
    binary[14] = '\0';

    reverse(binary);    
    return binary;
}

void reverse(char *str) {
    int i, j;
    char temp;
    for (i = 0, j = strlen(str) - 1; i < j; i++, j--) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}

/*******************************************************************/