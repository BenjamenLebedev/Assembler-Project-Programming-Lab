
#include "../Header_Files/ast.h"
#include "../Header_Files/global_var.h"
#include "../Header_Files/vector.h"
#include "../Header_Files/mid.h"
#include "../Header_Files/t_unit.h"
#include "../Header_Files/back.h" 
#include "../Header_Files/pre.h"
#include "../Header_Files/vector.h"
   
/**
 * @brief This program is an assembler for the assembly language described in the project.
 * the program recieves filenames of type .as but without the .as suffix.
 * the program will then assemble the files and create the following files: .ob, .ent, .ext.
 * ob - object file - a text file containing the machine code in base 4 encrypted (base 4 but with 0,1,2,3 replaced with * # % ! respectively) 
 * ent - entries file - contains the names of the symbols that were declared by .entry and the address of the lines which are labeled with those symbols.
 * ext - externals file - contains the names of the symbols that were declared by .extern and the address of the lines at which there's usage of those.
 * 
 * @param argc number of arguments - first argument is the name of the program. the rest are the names of the files to be assembled.
 * @param argv the array of the filenames.
 * @return * int 
 */
int main(int argc, char *argv[]) {
    int i;
    FILE *amFile;
    char *amFileName;
    void *const *begin;
    void *const *end;
    struct symbol *symbol; 
    struct translation_unit *translation_unit;

    if(argc < 2){
        printf("Error: to use the assembler at least one filename has to be provided\n");
        return 1;
    }

    for (i = 1; i < argc; i++){        
        
        printf("----------------------------------------------------------------------------------------------------- \n");
        printf("------------------------------  Assembling file: %s  started ---------------------------------------- \n", argv[i]);

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
        
        translation_unit = create_translation_unit();

        amFile = fopen(amFileName, "r");
        if(!amFile){
            printf("Error: file %s not found\n", amFileName);
            free(amFileName);
            free_translation_unit(translation_unit);
            return 1;
        }
        else{
            if(!firstPass(translation_unit, amFileName, amFile)){ 
                printf("********* printing symbols inside translation_unit:\n");
                VECTOR_LOOP(begin, end, translation_unit->symbols) {
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
                                printf("----------------------------------------------------------------------------------------------------- \n");
                                printf("--------------------------  Assembling file: %s.as finished successfully ---------------------------- \n", argv[i]);
                                printf("----------------------------------------------------------------------------------------------------- \n\n");
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

        free_translation_unit(translation_unit);
        free(amFileName);
    } /* end while loop over all argument files*/
    return 0;
}