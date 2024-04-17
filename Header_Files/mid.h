#ifndef MID_H
#define MID_H

#include "vector.h"
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

struct symbol{      /* תווית *//* hello: , x: ...*/
    int is_symbol_define;
    char symName[MAX_LABEL_LEN];
    int address;
    symType symType;
};  

struct ext{
    int address[1024];
    int address_count;
    char *ext_name;
};


/**
 * @brief constructor function for a symbol struct
 * 
 * @param copy pointer to the symbol we want to copy 
 * @return void* pointer to the new struct
 */
void *symbol_ctor(const void *copy);

/**
 * @brief destructor function for a symbol
 * 
 * @param item Pointer to the symbol structure to be destroyed
 */
void symbol_dtor(void *item);

/**
 * @brief constructor function for a extern struct
 * 
 * @param copy pointer to the extern we want to copy 
 * @return void* pointer to the new struct
 */
void *extern_ctor(const void *copy);

/**
 * @brief destructor function for a extern
 * 
 * @param item Pointer to the extern structure to be destroyed
 */
void extern_dtor(void *item);

/**
 * @brief checks if a symbol exists in the symbol vector
 * 
 * @param symbol_vector pointer to the vector of symbols
 * @param name pointer to the name of the symbol we want to search for
 * @return struct symbol* pointer to the symbol if found, else returns NULL
 */
struct symbol *does_symbol_exist(struct vector *symbol_vector, char *name);

/**
 * @brief checks if a extern exists in the extern vector
 * 
 * @param symbol_vector pointer to the vector of externs
 * @param name pointer to the name of the extern we want to search for
 * @return struct symbol* pointer to the extern if found, else returns NULL
 */
struct ext *does_extern_exist(struct vector *extern_vector, char *name);


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