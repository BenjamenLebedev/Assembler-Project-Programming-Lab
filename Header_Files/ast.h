#ifndef AST_H
#define AST_H

#include "global_var.h"

/*************************************************************************************/
/******************************Auxiliary structures***********************************/
/*************************************************************************************/

/*Structure to contain the information of the expression label[val]
with the val variable - being an offset - could be either a label
or an integer (legal integer that the assembler handles)*/
typedef struct offset_opt{ /* the offset of a certain data array - either string or int array */
    char *label; /*default value NULL*/
    int num; /*default value -1*/
} offset_opt;

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
    none_0 /* if the type represents nothing - meaning something failed*/
} address_0_type;

/*this structure saves all three possible options for a constant number 
being either 1 - an integer
2 - a label defined elsewhere as an integer*/
typedef struct address_0_op{
    address_0_type type;
    union{
        int num; /* just a number */
        char *label; /* a label of the number - defined with .define directive*/
    }option;
}address_0_op;

/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/

/*************************************************************************************/
/*******************************AST Structures****************************************/
/*************************************************************************************/

/* the type of the line in the assembly code */
typedef enum opt{
    inst, /* instruction that creates encoding depending on the operands*/
    dir,  /* directive that either directs to allocate data or handle external/entry data */
    define, /* a line that defines a symbol as a number */
    error,/* simply, a line that contains lexical errors, and therefore is invalid*/
    empty /* an empty line - either comprised only of white spaces or contain only a comment starting with ;*/
} line_type;
 
/* the legal operations (with the corresponding code numbers) for the assembler */
typedef enum opcode{
    op_mov,op_cmp,op_add,op_sub,op_not,op_clr,op_lea,op_inc,
    op_dec,op_jmp,op_bne,op_red,op_prn,op_jsr,op_rts,op_hlt
} inst_opcode;

/* the data types that the assembler handles */
typedef enum data_type{
    int_data, /* an simple integer within 12 bit boundaries */
    label_data, /* represents either a number or a different data*/
    string /* represents a string declared by .string directive*/
} data_type;

/* the data that the assembler handles - either an integer or a string
also an option is added to save the offset - in case it is used*/
typedef struct data{
    data_type type_data;
    union{
        char *label; /* string that represents a number defined elsewhere */
        int num; /* the number itself */
        char *string; /* string defined by the .string directive */
    }data_option;
    offset_opt offset; /* represents optional offset in case the operand is an array of data*/
}data;

/*saving the type of operands for the instruction - to make encoding easier*/
typedef enum address{
    const_num, /* the operand is a constant number */
    label, /* the operand is a label */
    label_offset, /* the operand is a label with an offset */
    reg, /* the operand is a register */
    none /*no operand*/
}address_type;

/* the operands for the instructions */
typedef struct inst_op{
    data data_inst;
    int reg_num; /* number of the register , -1 if not relevant*/
    address_type address_of_op; /* the address type of the operand */
} inst_operands[2];

/* the available directives in the given assembler code */
typedef enum dir{
    dir_entry, /* entry - declaration for use in other files*/
    dir_extern, /* extern - definition exists but in another file*/
    dir_data, /* data composed integers - positive and negative*/
    dir_string, /* declaring a string */
    dir_define /* defining a symbol as a number */
} dir_opcode;

/* the options for the operands of the directives */
typedef struct dir_op{
    data data_dir[DIR_DATA_SIZE]; /* the data of the directive */
    int num_count; /* the amount of operands */
} dir_operands;

/* The structure of the AST itself */
typedef struct ast_structure{
    char errors[ERROR_LEN]; /*NULL if no errors are found*/
    char label_of_line[MAX_LABEL_LEN + 1]; /*the label of the entire line*/
    line_type typeofLine; /*either instruction or directive*/

    union{ /*the operation code of either instruction or directive*/
        inst_opcode inst_code;
        dir_opcode  dir_code;
    } operation_code;

    union{ /*the operands of the operation - either for instruction or directive*/
        inst_operands inst_ops;
        dir_operands  dir_ops;
    } operands;

} ast_tree;

/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/

/*************************************************************************************/
/****************************AST Primary Functions************************************/
/*************************************************************************************/

/**
 * @brief this function receives a line from assembly code and returns the AST of the line.
 * performs lexical and syntactic analysis.
 * 
 * @param line the string which represents the line of assembly code.
 * @return frontend_ast* a pointer to the AST which describes the line of assembly code 
 */
ast_tree *ast_line(char* line);

/**
 * @brief This function initializes the AST structure.
 * 
 * @param ast the AST structure to be initialized.
 */
void init_ast(ast_tree *ast);

/**
 * @brief This function frees the memory allocated for the AST structure.
 * 
 * @param ast the AST structure to be freed.
 */
void free_ast(ast_tree *ast);



/**
 * @brief This function checks whether the line is a directive line, and if it is, it processes the directive in it.
 * 
 * @param ast the AST structure of the line into which the directive will be processed.
 * @param line the line of assembly code to be processed.
 * @return int 1 if the directive is legal, 0 otherwise.
 */
int check_directive(ast_tree *ast, char *line);

/**
 * @brief This function checks the legallity operands of the entry/extern directive. 
 * 
 * @param ast the AST structure of the assembley code line into which the directive will be processed.
 * @param line the line of assembly code to be processed. 
 * @return int 1 if the directive is legal, 0 otherwise.
 */
int check_entry_extern(ast_tree *ast, char *line);

/**
 * @brief This function checks the legallity for the operands of the .data directive. 
 * 
 * @param ast the AST structure of the assembley code line into which the directive will be processed.
 * @param line the line of assembly code to be processed.
 * @return int 1 if the directive is legal, 0 otherwise.
 */
int check_data_dir(ast_tree *ast, char *line);

/**
 * @brief This function checks the legallity for the operands of the .string directive. 
 * 
 * @param ast the AST structure of the assembley code line into which the directive will be processed.
 * @param line the line of assembly code to be processed. 
 * @return int 1 if the directive is legal, 0 otherwise.
 */
int check_string(ast_tree *ast, char *line);

/**
 * @brief This function checks the legallity for the operands of the .define directive. 
 * 
 * @param ast the AST structure of the assembley code line into which the directive will be processed.
 * @param line the line of assembly code to be processed. 
 * @return int 1 if the directive is legal, 0 otherwise.
 */
int check_define(ast_tree *ast, char *line);

/**
 * @brief This function checks whether the line is an instruction line, 
 * and if it is, it processes the instruction in it.
 * 
 * @param ast the AST structure of the line into which the instruction will be processed.
 * @param line the line of assembly code to be processed.
 * @return int 1 if the instruction is legal, 0 otherwise.
 */
int check_instruction(ast_tree *ast, char *line);

/**
 * @brief This function checks the legallity for the operands of the instruction. 
 * 
 * @param ast the AST structure of the assembley code line into which the directive will be processed.
 * @param line the line of assembly code to be processed. 
 * @param opcodeNum the number of the instruction in the array of the operations for instructions.
 * @return int 1 if the directive is legal, 0 otherwise.
 */
int check_inst_operands(ast_tree *ast, char *line,int opcodeNum);

/**
 * @brief This functions checks if the label at the beginning of the line is legal.
 * A legal label is defined as a string that starts with a letter and is followed by letters and digits only.
 * If the label is legal, it is returned by the function.
 * 
 * @param ast The AST of the line
 * @param line The line of assembly code
 * @param arg Flag that indicates whether the label an argument or it is a label of a line. 1 if it is an argument, 0 otherwise.
 * @return char* The label if it is legal, NULL otherwise.
 */
char *check_legal_label(ast_tree *ast, char *str,int arg);


/**
 * @brief This function tokenizes the string according to the delimiters and returns the next token. 
 * Similar to string.h strtok function but without using static variables.
 * 
 * @param str the string to be tokenized
 * @param delim the delimiters
 * @param saveptr a pointer designed to save the position of the next token
 * @return char* the next token
 */
char* my_strtok(char *str, const char *delim,char **saveptr);


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
 * @return *offset pointer to the legal offset structure if the check was legal, 
 * and an offset with label of NULL if it's not an offset, and NULL if memory allocation in it failed.
 */
offset *check_label_offset(ast_tree *ast, char *str);

/**
 * @brief Assigns the directive/instruction to the AST structure as a line_type enumeration.
 * 
 * @param ast the AST into which the directive/instruction will be assigned.
 * @param type the type of the line recieved - directive or instruction.
 * @param i the index of the directive/instruction in the directives list/operation codes array.
 */
void assign_ast_dir_inst(ast_tree *ast,line_type type,int i);



/**
 * @brief this function checks whether the recieved string is in the format of
 * #x - where x is either an integer,label, or an integer defined by array with offset.
 * if the string is in the correct format, the function returns an address_0_op struct
 * which contains all the information about the operand.
 * 
 * @param ast the AST of the assembly code line
 * @param str the string to be examined which possibly contains the desired format.
 * @return address_0_op pointer to the structure that contains all the information about an address zero type operand.
 * NULL if the string is not in the format.
 */
address_0_op *address_type_0(ast_tree *ast,char *str);

/**
 * @brief This function checks whether there is a newline character in the middle of the recieved string.
 * 
 * @param line the string to be examined.
 * @return int returns 0 if there is no newline character in the middle of the string, 1 otherwise.
 */
int check_mid_newline(char *line);


/*checks if the first character to appear (except white spaces)
is a semi colon
returns - 0 if a semi colon does not appear first
          1 if semi colon does appear first after white spaces
         -1 if the first character is semicolon, meaning it's a comment line*/
int is_first_semicolon(ast_tree *ast,char *line);

/**
 * @brief prints the AST - including all the substructures
 * 
 * @param ast the AST to be printed
 * @param line the line of the assembly code the AST describes
 */
void print_ast(ast_tree *ast,const char *line);

/*this function creates a char** pointer via memory allocation
 * for the my_strtok function.*/
char **create_saveptr(char **saveptr, char movptr[]);

/*like strcpy but with dynamic allocation of space in dest*/
char *copystr_calloc(ast_tree *ast, char* dest, const char *src);

/*freeing the provided offset structure*/
void free_offset_struct(offset *offset_var);

/*freeing the provided address_0_op structure*/
void free_op_address_0(address_0_op *operand_type_0);

/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/

/*************************************************************************************/
/********************MACROS - for better handling AST variables***********************/
/*************************************************************************************/

/*shortening part of the structure chain of the AST for the data of directives*/
#define DIR_OP_DATA(ast,index) (*ast).operands.dir_ops.data_dir[index]

/*shortening part of the structure chain of the AST for the data of instructions*/
#define INST_OP_DATA(ast,index) (*ast).operands.inst_ops[index].data_inst

#define FOUND_ALLOC_ERROR strstr(ast->errors,"Memory allocation")

#define FREE_SAVEPTR(saveptr,movptr) {\
    if(saveptr){\
        *saveptr = movptr;\
        free(saveptr);\
    }\
    }

#endif