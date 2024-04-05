#include "ast_lib.h"
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
#include "pre.h"

void print_ob(char *filename){
    printf("printing ob file for %s\n", filename);
}
void print_ent(char *filename){
    printf("printing ent file for %s\n", filename);
}
void print_ext(char *filename){
    printf("printing ext file for %s\n", filename);
}



int main(int argc, char *argv[]) {
    int i;
    FILE *amFile;
    char *amFileName;
/*     int result;
    void *const *begin;
    void *const *end;
    struct symbol *symbol; */

    struct translation_unit *translation_unit = (struct translation_unit*)malloc(sizeof(struct translation_unit));;
    translation_unit->symbols = new_vector(symbol_ctor, symbol_dtor);
    translation_unit->externals = new_vector(extern_ctor, extern_dtor);
    


    for (i = 1; i < argc; i++){

        amFileName = preprocessor(argv[i]);
        if(amFileName){
            printf("---------------------------------------------------------------------------------------------------- \n");
            printf("-------------------------------  preprocessor completed successfully ------------------------------- \n");
            printf("---------------------------------------------------------------------------------------------------- \n");
        }
        else{
            printf("---------------------------------------------------------------------------------------------------- \n");
            printf("      -------------------------------  preprocessor error ------------------------------- \n");
            printf("---------------------------------------------------------------------------------------------------- \n");
        }
        

        amFile = fopen(amFileName, "r");
        if(!amFile){
            printf("Error: file %s not found\n", amFileName);
            return 1;
        }
        else{
            if(firstPass(translation_unit, amFileName, amFile)){ 
                rewind(amFile); 
                if( secondPass(translation_unit, amFileName, amFile)){ 
                    print_ob(argv[i]);
                    if(1){
                        print_ent(argv[i]);
                    }
                    if(1){
                        print_ext(argv[i]);
                    }
                }
                else{
                    printf("Second pass completed with errors. on file\n\n\n");
                }
                
            }
            else{
                printf("First pass completed with errors. on file\n\n\n");
            }
            fclose(amFile);
        }
        /* free(amFileName); */ 
        vector_destroy(&translation_unit->symbols);
        vector_destroy(&translation_unit->externals);
    }
    return 0;
}

/*     result = firstPass(translation_unit, amFileName, amFile);
    if (result) {
        printf("First pass completed with errors. on first file\n\n\n");
    } else {
        printf("First pass completed successfully. on first file\n\n\n");
       
    }

    printf("********* printing symbols inside translation_unit:\n");
    VECTOR_FOR_EACH(begin, end, translation_unit->symbols) {
        if (*begin) {
            symbol = *begin;
            printf("********* symbol: %s, type: %d, address: %d\n", symbol->symName, symbol->symType, symbol->address);
        }
    }


    secondPass(translation_unit, amFileName, amFile);
    printf("********* printing code image:\n\n\n");
    print_code_image(translation_unit);

    printf("********* printing data image:\n\n\n");
    print_data_image(translation_unit);

    printf("********* printing symbols inside translation_unit:\n");
    VECTOR_FOR_EACH(begin, end, translation_unit->symbols) {
        if (*begin) {
            symbol = *begin;
            printf("********* symbol: %s, type: %d, address: %d\n", symbol->symName, symbol->symType, symbol->address);
        }
    }

    strcpy(amFileName , "text");
    i = make_ob_file(translation_unit, amFileName );
    if(i == 1){
        printf("back completed successfully");
    }
 */


/* int main(int argc, char *argv[]){

    FILE *amfile;
    char *amfileName;
    int i;

    for (i = 1; i < argc; i++){

        amfileName = preprocessor(argv[i]);

        amfile = fopen(amfileName, "r");
        if(!amfile){
            printf("Error: file %s not found\n", amfileName);
            return 1;
        }
        else{
            if(first_pass(amfile, amfileName)){ 
                rewind(amfile); 
                if(second_pass(amfile, amfileName)){ 
                    print_ob(argv[i]);
                    if(1){
                        print_ent(argv[i]);
                    }
                    if(1){
                        print_ext(argv[i]);
                    }
                }
                
            }
            fclose(amfile);
        }
        free(amfileName); 
    }
    return 0;
} */