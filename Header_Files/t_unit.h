
#ifndef T_UNIT_H
#define T_UNIT_H

struct translation_unit {
    int *code_image;
    int *data_image;
    int IC;
    int DC;
    struct vector *symbols;
    struct vector *externals;
};
 

/**
 * @brief Creates a new translation unit.
 * 
 * @return struct translation_unit* Pointer to the newly created translation unit.
 */
struct translation_unit *create_translation_unit();

/**
 * @brief Adds an integer value to the code image of the translation unit.
 * 
 * @param unit Pointer to the translation unit.
 * @param value Integer value to add.
 */
void add_to_code_image(struct translation_unit *unit, int value);

/**
 * @brief Adds an integer value to the data image of the translation unit.
 * 
 * @param unit Pointer to the translation unit.
 * @param value Integer value to add.
 */
void add_to_data_image(struct translation_unit *unit, int value);

void print_code_image(struct translation_unit *unit);
void print_data_image(struct translation_unit *unit);

void free_translation_unit(struct translation_unit *unit);
void printBinary14(int num);
#endif 
