#include "../Header_Files/ast.h"
#include "../Header_Files/global_var.h"

op_code_args op_codes1[] = {
    {"mov",  2}, 
    {"cmp",  2},  
    {"add",  2}, 
    {"sub",  2}, 
    {"not",  1},
    {"clr",  1},
    {"lea",  2}, 
    {"inc",  1}, 
    {"dec",  1},  
    {"jmp",  1}, 
    {"bne",  1}, 
    {"red",  1}, 
    {"prn",  1}, 
    {"jsr",  1}, 
    {"rts",  0}, 
    {"hlt",  0}  
};

int mainss(){

    frontend_ast *ast;
    char line1[] = "   MAIN:  .string \"asdasd\" \n";
    char *line;

    line = line1;

    /*“abcdef”*/

    ast = (frontend_ast *) calloc(1, sizeof(frontend_ast));
    if(ast == NULL){ 
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    *ast = frontend(line);

    /*print_ast(ast);*/

    frontend_free(ast);
    return 0;
}

void print_ast(frontend_ast *ast,char *line){

    int i;
    printf("The processed line of the AST is: [%s]\n", line);
    if((*ast).errors) printf("The AST errors are: %s\n", (*ast).errors);

    printf("The AST typeofLine is: %d ", (*ast).typeofLine);
    if((*ast).typeofLine == empty)      printf(" (empty) \n");
    else if((*ast).typeofLine == inst)  printf(" (inst) \n");
    else if((*ast).typeofLine == dir)   printf(" (dir) \n");
    else if((*ast).typeofLine == define)printf(" (define) \n");
    else if((*ast).typeofLine == error) printf(" (error) \n");

    if((*ast).typeofLine != error) printf("The AST label is: %s\n", (*ast).label);
    
    /* if we have an instruction, then print the operands of the instruction */
    if((*ast).typeofLine == inst){
        printf("The AST operation_code.inst_code is: %d", (*ast).operation_code.inst_code);
        for(i = 0; i < OPCODE_NUM; i++){
            if((*ast).operation_code.inst_code == i) printf(" (%s)\n", op_codes1[i].opcode);
        }

        for(i = 0; i < 2; i++){
            printf("The AST operands.inst_ops[%d].address_of_op is: %d", i, \
            (*ast).operands.inst_ops[i].address_of_op);
            if((*ast).operands.inst_ops[i].address_of_op == const_num)         printf(" (const_num)\n");
            else if((*ast).operands.inst_ops[i].address_of_op == label)        printf(" (label)\n");
            else if((*ast).operands.inst_ops[i].address_of_op == label_offset) printf(" (label_offset)\n");
            else if((*ast).operands.inst_ops[i].address_of_op == reg)          printf(" (reg)\n");
            else if((*ast).operands.inst_ops[i].address_of_op == none)         printf(" (none)\n");
            printf("The AST operands.inst_ops[%d].reg_num is: %d\n", i, (*ast).operands.inst_ops[i].reg_num);
            printf("The AST operands.inst_ops[%d].data_inst.type_data is: %d",i, \
            (*ast).operands.inst_ops[i].data_inst.type_data);
            if((*ast).operands.inst_ops[i].data_inst.type_data == int_data)        printf(" (int_data)\n");
            else if((*ast).operands.inst_ops[i].data_inst.type_data == label_data) printf(" (label_data)\n");
            else if((*ast).operands.inst_ops[i].data_inst.type_data == string)     printf(" (string)\n");
            
            if((*ast).operands.inst_ops[i].data_inst.type_data == int_data){
                printf("The AST operands.inst_ops[%d].data_inst.data_option.num is: %d\n", i, \
                (*ast).operands.inst_ops[i].data_inst.data_option.num);
            }
            else if((*ast).operands.inst_ops[i].data_inst.type_data == label_data){
                printf("The AST operands.inst_ops[%d].data_inst.data_option.label is: %s\n", i, \
                (*ast).operands.inst_ops[i].data_inst.data_option.label);
            }
            else if((*ast).operands.inst_ops[i].data_inst.type_data == string){
                printf("The AST operands.inst_ops[%d].data_inst.data_option.string is: %s\n", i, \
            (*ast).operands.inst_ops[i].data_inst.data_option.string);
            }
            printf("The AST operands.inst_ops[%d].data_inst.offset.label is: %s\n", i, \
            (*ast).operands.inst_ops[i].data_inst.offset.label);
            printf("The AST operands.inst_ops[%d].data_inst.offset.num is: %d\n", i, \
            (*ast).operands.inst_ops[i].data_inst.offset.num);
        }

    }

    /*dir_ops*/
    else if((*ast).typeofLine == dir || (*ast).typeofLine == define){
        printf("The AST operation_code.dir_code is: %d",  (*ast).operation_code.dir_code);
        if((*ast).operation_code.dir_code == dir_entry)       printf(" [dir_entry]\n");
        else if((*ast).operation_code.dir_code == dir_extern) printf(" [dir_extern]\n");
        else if((*ast).operation_code.dir_code == dir_data)   printf(" [dir_data]\n");
        else if((*ast).operation_code.dir_code == dir_string) printf(" [dir_string]\n");
        else if((*ast).operation_code.dir_code == dir_define) printf(" [dir_define]\n");
        printf("The AST operands.dir_ops.num_count is: %d\n", \
        (*ast).operands.dir_ops.num_count);
        for (i = 0; i < (*ast).operands.dir_ops.num_count; i++){
            printf("The AST operands.dir_ops.data_dir[%d].type_data is: %d",i, \
            (*ast).operands.dir_ops.data_dir[i].type_data);
            if((*ast).operands.dir_ops.data_dir[i].type_data == int_data)        printf(" (int_data)\n");
            else if((*ast).operands.dir_ops.data_dir[i].type_data == label_data) printf(" (label_data)\n");
            else if((*ast).operands.dir_ops.data_dir[i].type_data == string)     printf(" (string)\n");
            
            if((*ast).operands.dir_ops.data_dir[i].type_data == int_data){
                printf("The AST operands.dir_ops.data_dir[%d].data_option.num is: %d\n",i, \
                (*ast).operands.dir_ops.data_dir[i].data_option.num);
            }
            else if((*ast).operands.dir_ops.data_dir[i].type_data == label_data){
                printf("The AST operands.dir_ops.data_dir[%d].data_option.label is: %s\n",i, \
                (*ast).operands.dir_ops.data_dir[i].data_option.label);
            }
            else if((*ast).operands.dir_ops.data_dir[i].type_data == string){
                printf("The AST operands.dir_ops.data_dir[%d].data_option.string is: %s\n",i, \
                (*ast).operands.dir_ops.data_dir[i].data_option.string);
            }
            printf("The AST operands.dir_ops.data_dir[%d].offset.label is: %s\n",i, \
            (*ast).operands.dir_ops.data_dir[i].offset.label);
            printf("The AST operands.dir_ops.data_dir[%d].offset.num is: %d\n\n",i, \
            (*ast).operands.dir_ops.data_dir[i].offset.num);
        }
    }
}