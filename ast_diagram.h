/* the whole tree of the AST in one place */
#define MAX_LABEL_LEN 100
#define MAX_LINE_LEN 81

typedef struct frontend{
    char *errors;
    char label[MAX_LABEL_LEN + 1];
    typedef enum{
        inst, /* instruction that creates encoding depending on the operands*/
        dir,  /* directive that either directs to allocate data or handle external/entry data */
        define, /* a line that defines a symbol as a number */
        error,/* simply, a line that contains lexical errors, and therefore is invalid*/
        empty /* an empty line - either comprised only of white spaces or contain only a comment starting with ;*/
    } line_type;

    union{
        typedef enum{
            op_mov,op_cmp,op_add,op_sub,op_not,op_clr,op_lea,op_inc,
            op_dec,op_jmp,op_bne,op_red,op_prn,op_jsr,op_rts,op_hlt
        } inst_opcode;
        typedef enum{
            dir_entry, /* entry - declaration for use in other files*/
            dir_extern, /* extern - definition exists but in another file*/
            dir_data, /* data composed integers - positive and negative*/
            dir_string, /* declaring a string */
            dir_define /* defining a symbol as a number */
        } dir_opcode;
    } operation_code;

    union{
        struct{
            struct{
                typedef enum{
                    int_data, /* an simple integer within 12 bit boundaries */
                    label_data, /* represents either a number or a different data*/
                    string /* represents a string declared by .string directive*/
                }data_type;
                union{
                    char *label; /* string that represents a number defined elsewhere */
                    int num; /* the number itself */
                    char *string; /* string defined by the .string directive */
                }data_option;
                struct{ /* the offset of a certain data array - either string or int array */
                    char *label;
                    int num;
                }offset_opt;
            }data;

            int reg_num; /* number of the register , -1 if not relevant*/
            
            typedef enum {
                const_num, /* the operand is a constant number */
                label, /* the operand is a label */
                label_offset, /* the operand is a label with an offset */
                reg, /* the operand is a register */
                none
            }address_type;
        } inst_operands[2];

        struct{
            struct{
                typedef enum{
                    int_data, /* an simple integer within 12 bit boundaries */
                    label_data, /* represents either a number or a different data*/
                    string /* represents a string declared by .string directive*/
                } data_type;
                union{
                    char *label; /* string that represents a number defined elsewhere */
                    int num; /* the number itself */
                    char *string; /* string defined by the .string directive */
                }data_option;
                union{ /* the offset of a certain data array - either string or int array */
                    char *label;
                    int num;
                } offset_opt;
            }data[2*MAX_LINE_LEN];

            int num_count; /* the amount of numbers */
        } dir_operands;
    } operands;

} frontend_ast;