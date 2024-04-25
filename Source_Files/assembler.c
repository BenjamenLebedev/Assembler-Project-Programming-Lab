
#include "../Header_Files/lexer.h"
#include "../Header_Files/global_var.h"
#include "../Header_Files/vector_lib.h"
#include "../Header_Files/firstSecond_Pass.h"
#include "../Header_Files/t_unit.h"
#include "../Header_Files/code_convert.h" 
#include "../Header_Files/macro_spread.h"
#include "../Header_Files/vector_lib.h"
   
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
    struct translation_unit *translation_unit;

    if(argc < 2){
        printf("Error: to use the assembler at least one filename has to be provided\n");
        return 1;
    }

    for (i = 1; i < argc; i++){
        
        printf("-----  Assembly of file: %s  started \n\n", argv[i]);
        
        amFileName = macro_spread(argv[i]);

        if(!amFileName){
            printf("***** Error: macro spread process failed  \n");
            printf("***** since am file wasn't generated, this assembly process failed. \n");
            printf("***** moving on to next file, if there is one.\n\n");
            printf("-----  Assembly of file: %s  FAILED \n\n", argv[i]);
            continue;
        }
        
        translation_unit = create_translation_unit();

        amFile = fopen(amFileName, "r");
        if(!amFile){
            printf("***** error: Could not open file %s%s reading. moving on to the next file\n", amFileName,".am");
            free(amFileName);
            free_translation_unit(translation_unit);
            continue;
        }
        else{

            if(!firstPass(translation_unit, amFileName, amFile)){ 
                
                rewind(amFile); 

                if(!secondPass(translation_unit, amFileName, amFile)){ 

                    if(!gen_output_files(translation_unit,argv[i])){
                        printf("-----  Assembly of file: %s  finished successfully! \n\n\n", argv[i]);
                    }
                    else {
                        printf("\n-----  Assembly of file: %s FAILED: error generating one of the required output files.\n",argv[i]);
                        printf("-----moving on to next file\n\n\n");
                    }
                }
                else{
                    
                    printf("\n***** errors were encountered in the second pass of the assembly code\n");
                    printf("---- Assembly of file: %s FAILED!\n-----output files were not generated. moving on to next file\n\n\n",argv[i]);
                    
                    free(amFileName);
                    free_translation_unit(translation_unit);
                    continue;
                }
                
            }
            else{
                
                printf("\n***** errors were encountered in the first pass of the assembly code\n");
                printf("---- Assembly of file: %s FAILED!\n-----output files were not generated. moving on to next file\n\n\n",argv[i]);
                
                free(amFileName);
                free_translation_unit(translation_unit);
                continue;
            }

            fclose(amFile);
        }

        free_translation_unit(translation_unit);
        free(amFileName);
    } /* end while loop over all argument files*/
    return 0;
}