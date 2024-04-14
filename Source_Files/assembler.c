
#include "../Header_Files/ast.h"
#include "../Header_Files/global_var.h"
#include "../Header_Files/vector.h"
#include "../Header_Files/mid.h"
#include "../Header_Files/t_unit.h"
#include "../Header_Files/back.h" 
#include "../Header_Files/pre.h"
#include "../Header_Files/vector.h"
 
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
    void *const *begin;
    void *const *end;
    struct symbol *symbol; 
/*     int result;*/


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
            if(!firstPass(translation_unit, amFileName, amFile)){ 
                printf("********* printing symbols inside translation_unit:\n");
                VECTOR_FOR_EACH(begin, end, translation_unit->symbols) {
                    if (*begin) {
                    symbol = (struct symbol *) *begin;
                    printf("********* symbol: %s, type: %d, address: %d\n", symbol->symName, symbol->symType, symbol->address);
                    }
                }
                printf("---------------------------------------------------------------------------------------------------- \n");
                printf("---------------------------------  firstPass completed successfully -------------------------------- \n");
                printf("---------------------------------------------------------------------------------------------------- \n");
                rewind(amFile); 
                if(!secondPass(translation_unit, amFileName, amFile)){ 
                    printf("---------------------------------------------------------------------------------------------------- \n");
                    printf("---------------------------------  secondPass completed successfully -------------------------------- \n");
                    printf("---------------------------------------------------------------------------------------------------- \n");

                    printf("********* printing code image:\n\n\n");
                    print_code_image(translation_unit);
                    printf("********* printing data image:\n\n\n");
                    print_data_image(translation_unit); 

                    if(make_ob_file(translation_unit,argv[i])){
                        printf("---------------------------------------------------------------------------------------------------- \n");
                        printf("------------------------------------  Back completed successfully ---------------------------------- \n");
                        printf("---------------------------------------------------------------------------------------------------- \n");
                    }
                    else {
                        printf("---------------------------------------------------------------------------------------------------- \n");
                        printf("          -------------------------------  Back error ------------------------------- \n");
                        printf("---------------------------------------------------------------------------------------------------- \n");
                    }
/*                     print_ob(argv[i]);
                    if(1){
                        print_ent(argv[i]);
                    }
                    if(1){
                        print_ext(argv[i]);
                    } */
                }
                else{
                    printf("---------------------------------------------------------------------------------------------------- \n");
                    printf("        -------------------------------  secondPass error ------------------------------- \n");
                    printf("---------------------------------------------------------------------------------------------------- \n");
                }
                
            }
            else{
                printf("---------------------------------------------------------------------------------------------------- \n");
                printf("        -------------------------------  firstPass error ------------------------------- \n");
                printf("---------------------------------------------------------------------------------------------------- \n");
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