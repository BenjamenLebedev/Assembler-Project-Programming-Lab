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
    char symName[32];
    int address;
    symType symType;
};  

struct ext{
    int address[1024];
    int address_count;
    char *ext_name;
};



void *symbol_ctor(const void *copy);
void symbol_dtor(void *item);
void *extern_ctor(const void *copy);
void extern_dtor(void *item);
struct symbol *does_symbol_exist(struct vector *symbol_vector, char *name);
struct ext *does_extern_exist(struct vector *extern_vector, char *name);
void add_array_to_data_image(struct translation_unit *unit, int *array, size_t array_size);
int firstPass(struct translation_unit *translation_unit, char *amFileName, FILE *amFile);
int secondPass(struct translation_unit *translation_unit, char *amFileName, FILE *amFile);

#endif