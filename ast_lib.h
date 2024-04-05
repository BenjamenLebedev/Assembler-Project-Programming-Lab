#ifndef AST_LIB_H
#define AST_LIB_H

#include "ast_structs.h"
#include <stdlib.h> 
#include <stdio.h>
#include <string.h> 
#include <ctype.h>
#include <stdarg.h>

/*structure to save the number of allowed arguments for
each instruction operation*/
typedef struct op_args {
    char *opcode;    /* The legal operation code for the instructions */
    int arg_num;     /* The number of arguments permitted for the operation */
} op_code_args;

/*a struct that contains the information of an expression of the form
'label[val]' - with 'label' saved as a string named "label_array"
and the val being saved as offset structure - either a string or an integer*/
typedef struct offset{
    char *label_array; /* the label of the array */
    offset_opt offset_val; /* the offset of the array */
}offset;

/***************************************************************/
/* for correctly addressing instruction operand address type 0 */
typedef enum address_0_type{
    num, /* the operand is a constant number */
    label_0, /* the operand is a label */
    label_offset_0, /* the operand is a label with an offset */
    none_0 /* if the type represents nothing - meaning something failed*/
} address_0_type;

/*this structure saves all three possible options for a constant number 
being either 1 - an integer
2 - a label defined elsewhere as an integer
3 - an integer defined by and offset of a data array defined elsewhere*/
typedef struct address_0_op{
    address_0_type type;
    union{
        int num; /* just a number */
        char *label; /* a label of the number - defined with .define directive*/
        offset label_offset; /* a label with an offset - defined with .data directive*/
    }option;
}address_0_op;
/***************************************************************/


/**
 * @brief this function receives a line from assembly code and returns the AST of the line.
 * performs lexical and syntactic analysis.
 * 
 * @param line the string which represents the line of assembly code.
 * @return frontend_ast the AST which describes 
 */
frontend_ast frontend(char* line);

/**
 * @brief This function initializes the AST structure.
 * 
 * @param ast the AST structure to be initialized.
 */
void frontend_init(frontend_ast *ast);

/**
 * @brief This function frees the memory allocated for the AST structure.
 * 
 * @param ast the AST structure to be freed.
 */
void frontend_free(frontend_ast *ast);

/**
 * @brief This functions checks if the label at the beginning of the line is legal.
 * A legal label is defined as a string that starts with a letter and is followed by letters and digits only.
 * If the label is legal, it is returned by the function.
 * 
 * @param ast The AST of the line
 * @param line The line of assembly code
 * @param arg Flag that indicates whether the label an argument or it is a label of a line. 1 if it is an argument, 0 otherwise.
 * @param saveptr a pointer to the that is used during tokenization of a string.
 * @return char* The label if it is legal, NULL otherwise.
 */
char *check_legal_label(frontend_ast *ast, char *str,int arg,char **saveptr);

/**
 * @brief This function checks whether the line is a directive line, and if it is, it processes the directive in it.
 * 
 * @param ast the AST structure of the line into which the directive will be processed.
 * @param line the line of assembly code to be processed.
 * @param saveptr A pointer to the that is used during tokenization of a string.
 * @return int 1 if the directive is legal, 0 otherwise.
 */
int check_directive(frontend_ast *ast, char *line,char **saveptr);

/**
 * @brief This function checks whether the line is an instruction line, 
 * and if it is, it processes the instruction in it.
 * 
 * @param ast the AST structure of the line into which the instruction will be processed.
 * @param line the line of assembly code to be processed.
 * @param saveptr A pointer to the that is used during tokenization of a string.
 * @return int 1 if the instruction is legal, 0 otherwise.
 */
int check_instruction(frontend_ast *ast, char *line, char **saveptr);

/**
 * @brief this function checks whether a string is empty or not.+
 * 
 * @param str the string to be checked.
 * @return An integer. 1 if the string is empty. 0 otherwise.
 */
int isEmptyString(char* str);

/**
 * @brief This function trims the white spaces from the beginning and the end of the string, but not the middle of it (where are other characters).
 * 
 * @param str the string to be trimmed
 * @return char* the trimmed string
 */
char* trimStartEnd(char* str);

/**
 * @brief This function tokenizes the string according to the delimiters and returns the next token. Similar to string.h strtok function but
 * this function is thread safe - no use of static variables.
 * 
 * @param str the string to be tokenized
 * @param delim the delimiters
 * @param saveptr the pointer to the string that is being tokenized - saving the position of the next token. not to be altered to preserve tokenization.
 * @return char* the next token
 */
char* my_strtok(char *str, const char *delim,char **saveptr);

/**
 * @brief This function checks if the string is a register among 
 * the registers of the fictional machine of the assembler.
 * the check is done by comparing the string to the names of the registers.
 * if a match is found, the function returns the number of the register.
 * 
 * @param str the string to be checked
 * @return int the number of the register (from 0 to 7), -1 otherwise
 */
int is_reg(char* str);

/**
 * @brief This function checks if the string is an instruction among 
 * the instructions of the fictional machine of the assembler.
 * the check is done by comparing the string to the names of the instructions.
 * if a match is found, the function returns the number of the instruction (index in the op_codes array).
 * 
 * @param str the string to be checked
 * @return an integer \n int the number of the instruction (from 0 to 15), -1 if a match was not found, -2 if the received string is null.
 */
int is_inst(char* str);

/**
 * @brief This function checks if the string is a directive among 
 * the directives of the fictional machine of the assembler.
 * the check is done by comparing the string to the names of the directives.
 * if a match is found, the function returns the of the directive in the dir_list array.
 * 
 * @param str the string to be checked
 * @return integer, the index of directive in the dir_list array, -1 if a match was not found, -2 if the received string is null.
 */
int is_dir(char* str);

/**
 * @brief checks if any occurrence of a directive (data , string , etc..) is found in the string.
 * if such occurence is found, the function returns 1, otherwise 0.
 * 
 * @param str the string in which there's a possible occurence of a directive.
 * @return int 1 if a match to a dfirective was found, 0 otherwise.
 */
int find_dir(char *str);


/**
 * @brief This function checks whether the received string represents 
 * a legal integer. i.e. an integer in the accepted range of the fictional machine.
 * using 14 bits when an integer is used in directive lines and 12 bits when used in instruction lines.
 * 
 * @param str The string to be checked.
 * @param type the type of the line. directive or instruction.
 * @return int 1 if the string represents an integer, 0 otherwise.
 */
int is_integer(char* str,line_type type);

/**
 * @brief This function checks whether the recieved string has either the brackets [] 
 * or at least 2 of the " character. [] is for checking offset, " is for checking string directive.
 * 
 * @param str the string to be checked for brackets.
 * @param c the identifying character for the brackets. '[' for offset, '\"' for string directive.
 * @return int 1 if the string has the brackets as described, 0 otherwise.
 */
int check_brackets(char *str,char c);

/**
 * @brief This function checks whether the string is a legal label offset.
 * A legal label offset is defined as a legal label followed by brackets [] 
 * in which there's either a legal integer or a legal label.
 * If the label offset is legal, the function returns an offset struct that contains
 * all information about the offset.
 * 
 * @param ast The AST of the line
 * @param str The string containing the possible label offset.
 * @return offset the legal offset if the check was legal, and an offset with label of NULL otherwise.
 */
offset check_label_offset(frontend_ast *ast, char *str);

/**
 * @brief Assigns the directive/instruction to the AST structure as a line_type enumeration.
 * 
 * @param ast the AST into which the directive/instruction will be assigned.
 * @param type the type of the line recieved - directive or instruction.
 * @param i the index of the directive/instruction in the directives list/operation codes array.
 */
void assign_ast_dir_inst(frontend_ast *ast,line_type type,int i);

/**
 * @brief This function checks the legallity operands of the entry/extern directive. 
 * 
 * @param ast the AST structure of the assembley code line into which the directive will be processed.
 * @param line the line of assembly code to be processed. 
 * @param saveptr A pointer to the that is used during tokenization of a string.
 * @return int 1 if the directive is legal, 0 otherwise.
 */
int check_entry_extern(frontend_ast *ast, char *line, char **saveptr);

/**
 * @brief This function checks the legallity for the operands of the .data directive. 
 * 
 * @param ast the AST structure of the assembley code line into which the directive will be processed.
 * @param line the line of assembly code to be processed. 
 * @param saveptr A pointer to the that is used during tokenization of a string.
 * @return int 1 if the directive is legal, 0 otherwise.
 */
int check_data_dir(frontend_ast *ast, char *line, char **saveptr);

/**
 * @brief This function checks the legallity for the operands of the .string directive. 
 * 
 * @param ast the AST structure of the assembley code line into which the directive will be processed.
 * @param line the line of assembly code to be processed. 
 * @return int 1 if the directive is legal, 0 otherwise.
 */
int check_string(frontend_ast *ast, char *line);

/**
 * @brief This function checks the legallity for the operands of the .define directive. 
 * 
 * @param ast the AST structure of the assembley code line into which the directive will be processed.
 * @param line the line of assembly code to be processed. 
 * @param saveptr A pointer to the that is used during tokenization of a string.
 * @return int 1 if the directive is legal, 0 otherwise.
 */
int check_define(frontend_ast *ast, char *line, char **saveptr);

/**
 * @brief This function checks the legallity for the operands of the instruction. 
 * 
 * @param ast the AST structure of the assembley code line into which the directive will be processed.
 * @param line the line of assembly code to be processed. 
 * @param opcodeNum the number of the instruction in the array of the operations for instructions.
 * @param saveptr A pointer to the that is used during tokenization of a string.
 * @return int 1 if the directive is legal, 0 otherwise.
 */
int check_inst_operands(frontend_ast *ast, char *line,int opcodeNum, char **saveptr);

/**
 * @brief this function checks whether the recieved string is in the format of
 * #x - where x is either an integer,label, or an integer defined by array with offset.
 * if the string is in the correct format, the function returns an address_0_op struct
 * which contains all the information about the operand.
 * 
 * @param ast the AST of the assembly code line
 * @param str the string to be examined which possibly contains the desired format.
 * @param saveptr A pointer to the that is used during tokenization of a string.
 * @return address_0_op 
 */
address_0_op address_type_0(frontend_ast *ast,char *str, char **saveptr);


#endif