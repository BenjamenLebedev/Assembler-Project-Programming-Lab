
#include "../Header_Files/ast.h"
#include "../Header_Files/global_var.h"
#include "../Header_Files/vector.h"
#include "../Header_Files/mid.h"
#include "../Header_Files/t_unit.h"
#include "../Header_Files/back.h" 
#include "../Header_Files/pre.h"
#include "../Header_Files/vector.h"
   
  
int main(int argc, char *argv[]) {
    int i;
    FILE *amFile;
    char *amFileName;
    void *const *begin;
    void *const *end;
    struct symbol *symbol; 


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
                LOOP(begin, end, translation_unit->symbols) {
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

                    if(!make_ob_file(translation_unit,argv[i])){
                        printf("---------------------------------------------------------------------------------------------------- \n");
                        printf("------------------------------------  make_ob_file completed successfully -------------------------- \n");
                        printf("---------------------------------------------------------------------------------------------------- \n");
                        if(!make_extern_file(translation_unit,argv[i])){
                            printf("---------------------------------------------------------------------------------------------------- \n");
                            printf("-----------------------------  make_extern_file completed successfully ----------------------------- \n");
                            printf("---------------------------------------------------------------------------------------------------- \n");
                            
                            if(!make_entries_file(translation_unit,argv[i])){
                                printf("---------------------------------------------------------------------------------------------------- \n");
                                printf("-----------------------------  make_entries_file completed successfully ----------------------------- \n");
                                printf("---------------------------------------------------------------------------------------------------- \n");
                                printf("\n\n\n");
                                printf("---------------------------------------------------------------------------------------------------- \n");
                                printf("   -----------------------------  ALL completed successfully!!!! ----------------------------- \n");
                                printf("---------------------------------------------------------------------------------------------------- \n");
                            }  
                        }
                    }
                    else {
                        printf("---------------------------------------------------------------------------------------------------- \n");
                        printf("          -------------------------------  Back error ------------------------------- \n");
                        printf("---------------------------------------------------------------------------------------------------- \n");
                    }

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

        vector_destroy(&translation_unit->symbols);
        vector_destroy(&translation_unit->externals);
        free(translation_unit);
    }
    return 0;
}