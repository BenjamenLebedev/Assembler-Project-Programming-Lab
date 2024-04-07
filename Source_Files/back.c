#include <stdio.h>
#include <ctype.h>
#include <errno.h> 
#include <string.h>
#include <stdlib.h> 
#include "../Header_Files/vector.h"
#include "../Header_Files/mid.h"
#include "../Header_Files/global_var.h"
#include "../Header_Files/t_unit.h"
#include "../Header_Files/back.h"


void convertToSecretBase(int number, char secretBase[]) {

    int i;

    /* initialize the array with '*' */
    for (i = 0; i < 7; i++) {
        secretBase[i] = '*';
    }
    secretBase[7] = '\0'; 

    /* convert the number to secret base */
    i = 6; /* start from the end of the array */
    while (number > 0 && i >= 0) {
        switch (number % 4) {
            case 0:
                secretBase[i--] = '*';
                break;
            case 1:
                secretBase[i--] = '#';
                break;
            case 2:
                secretBase[i--] = '%';
                break;
            case 3:
                secretBase[i--] = '!';
                break;
        }
        number = number/4;
    }

}


int make_ob_file(const struct translation_unit *translation_unit, char *FileName){
    int i;
    char secretBase[8];
    int is_error = FALSE;

    char * file_ob_name;
    FILE *file_ob;
    char *ob_extension = ".ob";

    file_ob_name = (char *)malloc(strlen(FileName) + + strlen(ob_extension) + 1);
    strcpy(file_ob_name, FileName);
    strcat(file_ob_name, ob_extension);
    file_ob = fopen(file_ob_name, "w"); /*create file*/ 

    if(file_ob){
        for(i = 0; i<translation_unit->IC; i++){
            convertToSecretBase(translation_unit->code_image[i], secretBase);
            printf("IC: %s\n", secretBase);
            fprintf(file_ob, "%s\n", secretBase);
        }

        for(i = 0; i<translation_unit->DC; i++){
            convertToSecretBase(translation_unit->data_image[i], secretBase);
            printf("DC: %s\n", secretBase);
            fprintf(file_ob, "%s\n", secretBase);
        }
    }
    else{
        is_error = TRUE;
    }
    fclose(file_ob);
    free(file_ob_name);

    return is_error;

}