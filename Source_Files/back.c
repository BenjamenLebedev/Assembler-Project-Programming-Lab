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
    int mask,pair;


    /* initialize the array with '*' */ 
    if (number < 0) {
        /* number = -number - 1;  */
        for (i = 0; i < 7; i++) {  
            secretBase[i] = '!';
        }
    } else {
        for (i = 0; i < 7; i++) {
            secretBase[i] = '*';
        }
    }
    secretBase[7] = '\0'; 

    /* convert the number to secret base */
    i = 6; /* start from the end of the array */
    while (number > 0 && i >= 0) {
        mask =3;
        pair = number & mask;
        switch (pair) {
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
        number >>= 2;
    }
    while (number < 0 && i >= 0) {
        mask =3;
        pair = number & mask;
        switch (pair) {
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
        number >>= 2;
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
            printf("IC %d: %s\n",i+100, secretBase);
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


int make_extern_file(const struct translation_unit *translation_unit, char *FileName){\
    int i;
    int is_error = FALSE;
    void *const *begin;
    void *const *end;
    struct ext *external;

    char * file_ext_name;
    FILE *file_ext;
    char *ext_extension = ".ext";

    file_ext_name = (char *)malloc(strlen(FileName) + + strlen(ext_extension) + 1);
    strcpy(file_ext_name, FileName);
    strcat(file_ext_name, ext_extension);
    file_ext = fopen(file_ext_name, "w"); /*create file*/ 

    if(file_ext){
        printf("in extern if(file_ext) func\n");
        VECTOR_FOR_EACH(begin, end, translation_unit->externals) {
            if (*begin) {
                external = (struct ext *) *begin;
                printf("********* extern: %s, count: %d\n", external->ext_name, external->address_count); 

                for(i=0; i<external->address_count; i++){
                    fprintf(file_ext, "%s\t%d\n", external->ext_name, external->address[i]);
                }
            }
        }
    }
    fclose(file_ext);
    free(file_ext_name);

    return is_error;

}

int make_entries_file(const struct translation_unit *translation_unit, char *FileName){\
    int is_error = FALSE;
    void *const *begin;
    void *const *end;
    struct symbol *entrie;

    char * file_ent_name;
    FILE *file_ent;
    char *ent_extension = ".ent";

    file_ent_name = (char *)malloc(strlen(FileName) + + strlen(ent_extension) + 1);
    strcpy(file_ent_name, FileName);
    strcat(file_ent_name, ent_extension);
    file_ent = fopen(file_ent_name, "w"); /*create file*/ 

    if(file_ent){
        printf("in extern if(file_ext) func\n");
        VECTOR_FOR_EACH(begin, end, translation_unit->symbols) {
            if (*begin) {
                entrie = (struct symbol *) *begin;   
                if(entrie->symType == entryDataSymbol || entrie->symType == entryCodeSymbol ){
                    printf("********* entry: %s\n", entrie->symName); 
                    fprintf(file_ent, "%s\t%d\n", entrie->symName, entrie->address);
                }
            }
        }
    }
    fclose(file_ent);
    free(file_ent_name);
    return is_error;
}

