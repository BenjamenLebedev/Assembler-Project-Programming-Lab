#include "../Header_Files/ast.h"
#include "../Header_Files/global_var.h"
#include "../Header_Files/mid.h"

int main(int argc, char *argv[]){

    FILE *input;
    char filename[100];
    frontend_ast *ast = NULL;
    char *line1 = "MAIN: prn label[sz ]";
    char *line = NULL,c;
    int series;

    
    /*"STR: .data 5,3,2,1,x,yz,LIST[3]"*/

    series = 1;
    if(series == 1){

        if(argc < 2){
            printf("Error: no file name given\n");
            return 1;
        }
        
        strcpy(filename, argv[1]);
        input = fopen(argv[1], "r");
        if(!input){
            printf("Error: file %s not found\n", filename);
            return 1;
        }

        while((c = fgetc(input)) != EOF){
            line = read_line_input(c, input);
            ast = frontend(line);
            frontend_free(ast);
            free(line);
        }
        fclose(input);
    }
    else if(series == 0){
        line = (char *) calloc(strlen(line1) + 1, sizeof(char));
        if(!line){
            printf("Error: memory allocation failed\n");
            return 1;
        }
        strcpy(line, line1);
        ast = frontend(line);
        frontend_free(ast);
        free(line);
    }
    
    return 0;
}



void print_ast(frontend_ast *ast,const char *line){

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

    int i;
    printf("The processed line of the AST is: {%s}\n", line);
    if(!isEmptyString((*ast).errors)){
        printf("The AST errors are: %s\n", (*ast).errors);
        return;
    }

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
            printf("********************operand no. %d**************************\n",i+1);
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
            
            if((*ast).operands.inst_ops[i].data_inst.offset.label){
                printf("The AST operands.inst_ops[%d].data_inst.offset.label is: %s\n", i, \
                (*ast).operands.inst_ops[i].data_inst.offset.label);
            }
            else if((*ast).operands.inst_ops[i].data_inst.offset.num != -1){
                printf("The AST operands.inst_ops[%d].data_inst.offset.num is: %d\n", i, \
                (*ast).operands.inst_ops[i].data_inst.offset.num);
            }
        }
        printf("\n");
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
            printf("********************operand no. %d**************************\n",i+1);
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

            if((*ast).operands.dir_ops.data_dir[i].offset.label){
                printf("The AST operands.dir_ops.data_dir[%d].offset.label is: %s\n",i, \
                (*ast).operands.dir_ops.data_dir[i].offset.label);
            }
            else if((*ast).operands.dir_ops.data_dir[i].offset.num != -1){
                printf("The AST operands.dir_ops.data_dir[%d].offset.num is: %d\n",i, \
                (*ast).operands.dir_ops.data_dir[i].offset.num);
            }
        }
        printf("\n");
    }
}