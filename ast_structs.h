#ifndef AST_STRUCT_H
#define AST_STRUCT_H

#include "global_var.h"

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

/*Structure to contain the information of the expression label[val]
with the val variable - being an offset - could be either a label
or an integer (legal integer that the assembler handles)*/
typedef struct offset_opt{ /* the offset of a certain data array - either string or int array */
    char *label; /*default value NULL*/
    int num; /*default value -1*/
} offset_opt;

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
    data data_dir[2*MAX_LINE_LEN]; /* the data of the directive */
    int num_count; /* the amount of operands */
} dir_operands;


/* The structure of the AST itself */
typedef struct frontend{
    char *errors; /*NULL if no errors are found*/
    char label[MAX_LABEL_LEN + 1]; /*the label of the entire line*/
    line_type typeofLine; /*either instruction or directive*/

    union{ /*the operation code of either instruction or directive*/
        inst_opcode inst_code;
        dir_opcode  dir_code;
    } operation_code;

    union{ /*the operands of the operation - either for instruction or directive*/
        inst_operands inst_ops;
        dir_operands  dir_ops;
    } operands;

} frontend_ast;


#endif