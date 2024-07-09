#ifndef GLOBAL_VAR_H
#define GLOBAL_VAR_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h> 
#include <stdarg.h>
#include <stddef.h>

enum {
    FALSE,
    TRUE
};

typedef enum symFlag{ 
        NON_EXTERN,
        EXTERN
} symFlag;

/*structure to save the number of allowed arguments for
each instruction operation*/
typedef struct op_args {
    char *opcode;    /* The legal operation code for the instructions */
    int arg_num;     /* The number of arguments permitted for the operation */
} op_code_args;

extern op_code_args op_codes[16];
extern char *dir_list[5];

/*The starting value of the instruction counter*/
#define IC_START 100

/*the amount of memory in the fictional machine*/
#define MEM_SIZE 4096

/* Maximal length for a single line of command*/
#define MAX_LINE_LEN 82

/* Maximal length of a label */
#define MAX_LABEL_LEN 31

/* length of each word in command line, in bits */
#define WORD_LEN 14

/* Length of an error string */
#define ERROR_LEN 200

/* Total number of registers allowed in the system*/
#define REG_NUM 8

/* the number of instruction operation codes */
#define OPCODE_NUM 16

/* the number of legal directives for the assembler */
#define DIR_NUM 5

/*the maximal number of arguments in the .data directive*/
#define DIR_DATA_SIZE MAX_LINE_LEN/2

/*the amount of characters read from input with each realloc*/
#define CHUNK_READ 300

/*all the characters that are white spaces (save for newline character)*/
#define SPACES  " \t\v\f"

/* the maximal and minimal values of integer in the instructions for the fictional machine 
in the instructions we have the first 2 bits reserved (A.R.E) so here we go with only 12 bits
meaning 12-1 = 14-3 --> since we support negative integers we need the last bit for sign representation*/
#define MAX_INT_INST ((1 << (WORD_LEN-3)) - 1) /*2047*/
#define MIN_INT_INST -(1 << (WORD_LEN-3)) /*-2048*/

/* the maximal and minimal values of integer in the directives for the fictional machine 
in the directives we use all 14 bits for the integer so the values are like this*/
#define MAX_INT_DIR ((1 << (WORD_LEN-1)) - 1) /*8191*/
#define MIN_INT_DIR -(1 << (WORD_LEN-1)) /*-8192*/

/* the maximal amount of offset - since in .data directive we can't have more than
37 numbers at most (single digit numbers) - since the line is confined to 82 characters (including newline character)*/
#define MAX_OFFSET 36


/**************************************************************************************/
/************Global functions - used by nore than one of the source files**************/
/**************************************************************************************/

/**
 * @brief reads a line from the input file until a new line character is encountered or the end of the file is reached.
 * 
 * @param c the first character of the line. serves to check EOF is reached.
 * @param file the file from which we read the line.
 * @return char* string containing the line read from the input.
 */
char* read_line_input(char c,FILE* file);

/**
 * @brief This function trims the white spaces from the beginning and the end of the string, but not the middle of it (where are other characters).
 * 
 * @param str the string to be trimmed
 * @return char* the trimmed string
 */
char* trimStartEnd(char* str);

/**
 * @brief This function checks if the string is a register among 
 * the registers of the fictional machine of the assembler.
 * the check is done by comparing the string to the names of the registers.
 * if a match is found, the function returns the number of the register.
 * 
 * @param str the string to be checked
 * @return int the number of the register (from 0 to 7), 
 * -1 if a match was not found
 * -2 if the received string is null.
 */
int is_reg(char* str);

/**
 * @brief This function checks if the string is an instruction among 
 * the instructions of the fictional machine of the assembler.
 * the check is done by comparing the string to the names of the instructions.
 * if a match is found, the function returns the number of the instruction (index in the op_codes array).
 * 
 * @param str the string to be checked
 * @return an integer \n int the number of the instruction (from 0 to 15), 
 * -1 if a match was not found, 
 * -2 if the received string is null.
 */
int is_inst(char* str);

/**
 * @brief This function checks if the string is a directive among 
 * the directives of the fictional machine of the assembler.
 * the check is done by comparing the string to the names of the directives.
 * if a match is found, the function returns the of the directive in the dir_list array.
 * 
 * @param str the string to be checked
 * @return integer, the index of directive in the dir_list array, 
 * -1 if a match was not found, 
 * -2 if the received string is null.
 */
int is_dir(char* str);

/**
 * @brief this function checks whether a string is empty or not.
 * 
 * @param str the string to be checked.
 * @return An integer. 1 if the string is empty. 0 otherwise.
 */
int isEmptyString(char* str);


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
void sym_dtor(void *item);

/**
 * @brief constructor function for a extern struct
 * 
 * @param copy pointer to the extern we want to copy 
 * @return void* pointer to the new struct
 */
void *extern_ctor(const void *copy);


#endif