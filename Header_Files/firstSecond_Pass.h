#ifndef FIRSTSECOND_PASS_H
#define FIRSTSECOND_PASS_H

#include "vector_lib.h"
#include "global_var.h" 
#include "t_unit.h"



typedef enum symType{ 
        entrySymbol,
        externSymbol, 
        codeSymbol,
        dataSymbol,
        entryCodeSymbol,
        entryDataSymbol 
} symType;

struct symbol{      /* a label *//* hello: , x: ... or variable name like in define directive*/
    int is_symbol_define; /*is the symbol part of .define symbol = num*/
    int num_line_defined; /*the line number of the .define directive*/
    int define_val; /*the value of the symbol in case it is a .define symbol = define_val*/
    int len_data; /*amount of elements in case it is a dataSymbol of data directive*/
    int len_string; /* length of the string in case it is a dataSymbol of string directive*/
    char symName[MAX_LABEL_LEN];
    int address;
    symType symType;
};  

struct ext{
    int address[MEM_SIZE/4];
    int address_count;
    char ext_name[MAX_LABEL_LEN];
};

/**
 * @brief initializes a symbol struct
 * 
 * @param symbol pointer to the symbol struct
 * @return struct symbol* pointer to the symbol struct
 */
struct symbol * init_symbol(struct symbol *symbol);

/**
 * @brief checks if a symbol exists in the symbol/extern vector
 * 
 * @param symbol_vector pointer to the vector of symbols
 * @param name pointer to the name of the symbol we want to search for
 * @param flag flag to indicate the type of symbol we are looking for - whether it is an external symbol or non external symbol
 * @return void* pointer to the symbol if found, else returns NULL
 */
void *does_sym_exist(void *sym, char * name, symFlag flag);

/**
 * @brief adds an array of integers to the data image in the translation unit
 * 
 * @param unit pointer to the translation unit
 * @param array pointer to the array of integers
 * @param array_size size of the array
 */
void add_array_to_data_image(struct translation_unit *unit, int *array, size_t array_size);

/**
 * @brief performs the first pass,
 * This function reads the input file line by line, analyzing each line to identify symbols and their types,
 * addresses, and other information. It processes different types of lines such as instructions, data declarations, and
 * label definitions, updating the instruction counter (IC) and data counter (DC) accordingly. Additionally, it handles
 * semantic errors and keeps track of defined and declared symbols.
 * 
 * @param translation_unit pointer to the translation unit structure
 * @param amFileName name of the input assembly file 
 * @param amFile file pointer to the input assembly file
 * @return int A flag indicating whether any errors occurred during the first pass 
 * (TRUE if errors occurred, FALSE otherwise)
 */
int firstPass(struct translation_unit *translation_unit, char *amFileName, FILE *amFile);

/**
 * @brief Performs the second pass,
 * the function reads the assembly file line by line, parsing each line into an
 * abstract syntax tree (AST) and processing each instruction. It updates the
 * code image in the `translation_unit` as necessary and handles data and label
 * operations.
 *
 * @param translation_unit pointer to the translation_unit struct
 * @param amFileName string representing the name of the assembly file being processed.
 * @param amFile a file pointer to the assembly file being processed.
 *
 * @return int flag indicating whether any errors occurred during the first pass
 *  (TRUE if errors occurred, FALSE otherwise)
 */
int secondPass(struct translation_unit *translation_unit, char *amFileName, FILE *amFile);

#endif