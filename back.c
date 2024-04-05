#include <stdio.h>
#include <ctype.h>
#include <errno.h> 
#include <string.h>
#include <stdlib.h> 
#include "vector.h"
#include "mid.h"
#include "ast_structs.h"
#include "t_unit.h"
#include "back.h"


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
        return 0;
    }
    fclose(file_ob);
    free(file_ob_name);

    return 1;

}