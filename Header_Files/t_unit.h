#ifndef T_UNIT_H
#define T_UNIT_H

struct translation_unit { 
    int *code_image;
    int *data_image;
    int IC; /* Instruction Counter */
    int DC; /* Data Counter */
    struct vector *symbols; /*Vector that stores the entry symbols*/
    struct vector *externals; /*Vector that stores the external symbols and the addresses at which each symbol is used*/
    
    int extern_use; /*flag to indicate whether an external symbol is used in current file*/
    int entry_use; /*flag to indicate whether there are entry symbols*/
};
 

/**
 * @brief creates a new translation unit.
 * 
 * @return struct translation_unit* Pointer to the newly created translation unit.
 */
struct translation_unit *create_translation_unit();

/**
 * @brief adds an integer to the code image of the translation unit.
 * 
 * @param unit pointer to the translation unit.
 * @param value int value to add.
 */
void add_to_code_image(struct translation_unit *unit, int value);

/**
 * @brief adds an integer to the data image of the translation unit.
 * 
 * @param unit pointer to the translation unit.
 * @param value int value to add.
 */
void add_to_data_image(struct translation_unit *unit, int value);

/**
 * @brief function that frees the memory of the transaltion unit
 * 
 * @param unit the translation unit we want to delete and free it memory.
 */
void free_translation_unit(struct translation_unit *unit);

void print_code_image(struct translation_unit *unit);
void print_data_image(struct translation_unit *unit);
void printBinary14(int num);
#endif 
