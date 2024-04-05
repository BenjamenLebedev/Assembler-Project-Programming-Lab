#include "ast_lib.h" 

/* the instruction operation codes and the permitted number of arguments for each*/
op_code_args op_codes[] = {
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

/* Define the directives - for effective comparison */
char *dir_list[] = {"entry" , "extern" , "data" , "string" , "define"};

char *check_legal_label(frontend_ast *ast, char *str,int arg,char **saveptr){

    char *token,*label;
    int i,error_flag; /*error_flag - 1 means error exists. 0 means not*/
    
    /* extracting the part of the label, before a colon - if it's a label at the start of the line*/
    if(!arg){
        token = my_strtok(str, ":", saveptr);
        if(isEmptyString(token)){ /* if nothing is before the colon*/
            strcpy((*ast).errors, "Missing label declaration before colon\n");
            return NULL;
        } /* if between the colon and the label there's a space*/
        else if(isspace(token[strlen(token) - 1])){
            strcpy((*ast).errors, "declaration of label must be adjacent to colon without spaces\n");
            return NULL;
        }
    } 
    else token = str; /*if not - simply check the string that it's a legal label*/
    token = trimStartEnd(token);

    error_flag = 0;
    /* checking errors - if the string is not a system reserved word */
    if(is_reg(token) >= 0 || is_inst(token) >= 0 || is_dir(token) >= 0 || \
    !strcmp(token, "mcr") || !strcmp(token, "endmcr")){
        if (!arg) strcpy((*ast).errors, "A system reserved word (like mov , data , etc...) cannot be used as a label.\n");
        return NULL;
    }
    /* legal label - starts with an alphabet letter and it's length is within bounds (31 characters)*/
    if(strlen(token) <= MAX_LABEL_LEN && isalpha(token[0])){
        for (i = 1; token[i]; i++){
            /*if we have something that's not a letter or a digit*/
            if(!isalnum(token[i])){ 
                error_flag = 1;
                break;
            }
        }
    }
    else error_flag = 1;

    if(!error_flag) label = token; /*saving the label if it was found without errors*/
    else label = NULL;

    if(!arg){ /*if it's a line label (before a colon), checking what is after that*/
        token = my_strtok(NULL, ":", saveptr);
        /*if there's no space after the colon it's an error*/
        if(token && token[0] != ' '){ 
            strcpy((*ast).errors, "Missing space after label declaration\n");
            return NULL;
        }
        /*if there's nothing after the colon, it's a label on an empty line which is illegal*/
        if(!token || isEmptyString(trimStartEnd(token))){
            strcpy((*ast).errors, "Label cannot be declared on empty line\n");
            return NULL;
        }
    }
    
    return label;
}

/**************************************************************************************/
/**************************************************************************************/
/*****************************Functions for directives*********************************/
/**************************************************************************************/
/**************************************************************************************/

int find_dir(char *str){
    int i;
    char *ptr;

    ptr = str + 1; /*skipping the point before the directive word*/
    for(i = 0; i < DIR_NUM && str[0] == '.'; i++){
        /*if an instance of a directive word was found*/
        if(strstr(ptr,dir_list[i])) return 1; 
    }
    return 0;
}

int check_directive(frontend_ast *ast, char *line,char **saveptr){
    char *token;
    int i,j,check_args;

    /* extracting the part of the directive, after a dot */
    token = my_strtok(line, " ", saveptr);
    line += strlen(token) + 1; /* skipping the directive for checking operands later */
    line = trimStartEnd(line);
    
    i = -1;
    /* trimming start and end spaces and skipping the point at the start*/
    token = trimStartEnd(token); 
    token++;
    /* Identifying the directive and assigning the correct operation code into the AST*/
    i = is_dir(token);
    if(i >= 0){ /*only assigning directive type if alegal directive was found*/
        (*ast).typeofLine = dir;
        assign_ast_dir_inst(ast,dir,i);
    }
    else if(i == -1){
        strcpy((*ast).errors, "Illegal directive\n");
        return 0;
    }

    /*initializing offset values for all the arguments*/
    for(j = 0; j < 2*MAX_LINE_LEN; j++){
        (*ast).operands.dir_ops.data_dir[j].offset.label = NULL;
        (*ast).operands.dir_ops.data_dir[j].offset.num = -1;
    }

    /* Refering to the relevant function to check the operands of the directives*/
    switch ((*ast).operation_code.dir_code){   
    case dir_entry:
    case dir_extern:
        check_args = check_entry_extern(ast, line, saveptr);
        break;
    case dir_data:
        check_args = check_data_dir(ast, line, saveptr);
        break;
    case dir_string:
        check_args = check_string(ast, line);
        break;
    case dir_define:
        check_args = check_define(ast, line, saveptr);
        break;
    }

    if(!check_args) return 0;

    return 1;
}

int check_entry_extern(frontend_ast *ast, char *line, char **saveptr){
    
    char *token, *check_label;
    int index;
    
    token = my_strtok(line, " ", saveptr);
    /* if there's nothing in the recieved string, then we have no operand */
    if(!token || isEmptyString(token)){
        strcpy((*ast).errors, "Missing label after entry/extern directive\n");
        return 0;
    }

    token = trimStartEnd(token);
    /*checking the argument label*/
    check_label = check_legal_label(ast, token, 1,saveptr);
    
    if(!check_label){
        strcpy((*ast).errors, "Illegal label after entry/extern directive\n");
        return 0;
    }

    index = (*ast).operands.dir_ops.num_count;
    /*inserting the label into the ast*/
    (*ast).operands.dir_ops.data_dir[index].type_data = label_data;
    (*ast).operands.dir_ops.data_dir[index++].data_option.label = check_label;
    (*ast).operands.dir_ops.num_count = index;

    /* extra arguments after the label */
    if((token = my_strtok(NULL, " ", saveptr))){
        strcpy((*ast).errors, "Too many arguments for directive entry/extern\n");
        return 0;
    }

    /*if we have a line label - it is meaningless for entry/extern so
    we delete it to prevent confusion*/
    if(!isEmptyString((*ast).label)) (*ast).label[0] = '\0';
    
    return 1;
}

int check_data_dir(frontend_ast *ast, char *line, char **saveptr){

    char *token,*token_ind,*is_label;
    char token_backup[MAX_LINE_LEN],*delim = ",";
    int is_int,index,i;
    offset is_label_offset;

    /*starting to check each operand between commas*/
    token = my_strtok(line,delim, saveptr);
    i = 0;
    token_ind = line;
    while(token){
        /*saving the backup token to properly check commas between different tokens*/
        strcpy(token_backup,token);

        token = trimStartEnd(token);
        /* token empty (means no operand) checking different cases, either commas at the end or something else*/
        if(isEmptyString(token)){
            /*no first operand - if no comma is after that, the operand is simply missing*/
            if(i == 0 && !strchr(token_ind,',')) strcpy((*ast).errors, "Missing arguments for .data directive\n");
            /*no first operand - there's a comma after*/
            if(i == 0 && strchr(token_ind,',')) strcpy((*ast).errors, "Missing first argument before comma for .data directive\n");
            /*if there's a comma before and after and no operand*/
            else if(i > 0 && strchr(token_ind,','))  strcpy((*ast).errors, "Consecutive commas\n");
            /*if there's a comma before, no comma after and no operand*/
            else if(i > 0 && !strchr(token_ind,',')) strcpy((*ast).errors, "Missing argument after comma\n");
            return 0;
        }
        /*the options for each operand*/
        is_int = is_integer(token,dir);/*just an integer*/
        /*a label defined as integer elsewhere*/
        is_label = check_legal_label(ast,token,1,saveptr);
        /*an integer represented by an expression label[something] - 
        meaning represented by the result of a data array offset*/
        is_label_offset = check_label_offset(ast,token);

        /*if the operand matched none of the 3 allowed types*/
        if(!is_int && !is_label && !is_label_offset.label_array){ /* token is not a variable name or a legal integer - either not a number or the integer is out of bounds */
            strcpy((*ast).errors, "Illegal data argument\n");
            return 0;
        }

        /*if the operand's legal - save it into the AST */
        index = (*ast).operands.dir_ops.num_count;
        if(is_int){ /*integer*/
            (*ast).operands.dir_ops.data_dir[index].type_data = int_data;
            (*ast).operands.dir_ops.data_dir[index++].data_option.num = atoi(token);
        } 
        else if(is_label){ /*integer represented by a label*/
            (*ast).operands.dir_ops.data_dir[index].type_data = label_data;
            (*ast).operands.dir_ops.data_dir[index++].data_option.label = token;
        }
        else if(is_label_offset.label_array){ /*offset value representation*/
            (*ast).operands.dir_ops.data_dir[index].type_data = label_data;
            (*ast).operands.dir_ops.data_dir[index].data_option.label = is_label_offset.label_array;
            (*ast).operands.dir_ops.data_dir[index++].offset = is_label_offset.offset_val;
        }
        else{ /*printing error if the operand isn't legal*/
            strcpy((*ast).errors, "Illegal data argument as offset of a data array\n");
            return 0;
        }
        /*increasing the total number of operands between iterations*/
        (*ast).operands.dir_ops.num_count = index;
        
        /*advancing the token_ind pointer to be at the start of the next token*/
        if(++i > 0 && strchr(token_ind,',')) token_ind += (int)strlen(token_backup) + 1;
        token = my_strtok(NULL, ",", saveptr);
    }

    return 1;
}

int check_string(frontend_ast *ast, char *line){
    char *start,*end,*str;
    int i,j,index,len;

    /*if the operand is empty*/
    if(!line || isEmptyString(line)){
        strcpy((*ast).errors, "Missing string argument\n");
        return 0;
    }
    /*if there's less the 2 sets of double quotes*/
    if(!strchr(line,'\"') || !check_brackets(line,'\"')){
        strcpy((*ast).errors, "String declared in .string directive must be enclosed in double quotes\n");
        return 0;
    }

    len = strlen(line);
    start = end = NULL;
    i = 0;
    j = len - 1;

    /*checking the string for legal characters and the double quotes*/
    while (i < len && i < j){
        /*finding the first and last double quotes*/
        if(line[i] == '\"' && !start) start = line + i + 1;
        if(line[j] == '\"' && !end)   end   = line + j;
        
        /*if we found any printable character before finding 
        the first and last double quotes, it's illegal since the 
        whole string needs to be enclosed in double quotes*/
        if((isprint(line[i]) && !isspace(line[i]) && !start) \
        || (isprint(line[j]) && !isspace(line[j]) && !end)){
            strcpy((*ast).errors, "string in .string directive not enclosed in double quotes\n");
            return 0;
        }
        
        /*if we first and last double quotes but the character is unprintable
        we exit the loop*/
        if(start && end && (!isprint(line[i]) || !isprint(line[j])) ) break;

        i++;
        j--;
    }

    /*the only way the string is legal is if we went through it all
    and we got to the middle of the string without errors*/
    if(!(i == j || i == j + 1)){
        strcpy((*ast).errors, "Illegal string in .string directive\n");
        return 0;
    }
    
    /*saving only part between the first and last double quotes
    into str*/
    str = (char*) calloc(end - start + 1,sizeof(char));
    if(!str){
        strcpy((*ast).errors, "Memory allocation failed\n");
        return 0;
    }
    memcpy(str,start,end - start);
    str[end - start] = '\0';

    /*saving the string into the AST*/
    index = (*ast).operands.dir_ops.num_count;

    (*ast).operands.dir_ops.data_dir[index].type_data = string;
    (*ast).operands.dir_ops.data_dir[index++].data_option.string = str;
    (*ast).operands.dir_ops.num_count = index;

    return 1;
}


int check_define(frontend_ast *ast, char *line, char **saveptr){

    char *token,*label;
    char *delim = "=";
    int index,num,check_num;

    /* if a legal line label was found before a define line of .define directive - it's an error*/
    if(!isEmptyString((*ast).label)){
        strcpy((*ast).errors, "A label cannot be declared on a line with .define directive\n");
        return 0;
    }

    /* first token - the symbol*/
    token = my_strtok(line, delim, saveptr);
    token = trimStartEnd(token);

    if(!token || isEmptyString(token)){ /*empty token - no symbol*/
        strcpy((*ast).errors, "Missing symbol after .define directive\n");
        return 0;
    }

    label = NULL; /*checking the legality of the symbol - like a label*/
    if(!(label = check_legal_label(ast, token, 1,saveptr))){
        strcpy((*ast).errors, "Illegal symbol after .define directive\n");
        return 0;
    }

    /* second token - the number assigned to the symbol */
    token = my_strtok(NULL, delim, saveptr);
    token = trimStartEnd(token);

    if(!token || isEmptyString(token)){ /*no number after symbol*/
        strcpy((*ast).errors, "Missing integer in .define directive\n");
        return 0;
    }
    if(!(check_num = is_integer(token,dir))){ /*if the number operand is illegal*/
        strcpy((*ast).errors, "Illegal integer operand in .define directive\n");
        return 0;
    }
    else num = atoi(token); /*saving the number*/

    /*only if both operands are legal, we save them into the AST*/
    if(label && check_num){ 
        index = (*ast).operands.dir_ops.num_count;

        (*ast).operands.dir_ops.data_dir[index].type_data = label_data;
        (*ast).operands.dir_ops.data_dir[index++].data_option.label = label;
        (*ast).operands.dir_ops.data_dir[index].type_data = int_data;
        (*ast).operands.dir_ops.data_dir[index++].data_option.num = num;
        (*ast).operands.dir_ops.num_count = index;
    }
    
    return 1;
}

/**************************************************************************************/
/**************************************************************************************/
/****************************Functions for instructions********************************/
/**************************************************************************************/
/**************************************************************************************/


int check_instruction(frontend_ast *ast, char *line, char **saveptr){
    char *token;
    int i,j;

    /*first token - the instruction operation code (mov,add, etc...)*/
    token = my_strtok(line, " ", saveptr);
    line += strlen(token) + 1; /* skipping the instruction for checking operands later */
    line = trimStartEnd(line);
    
    i = -1; /*initializing value to check type of operation*/
    /* Identifying the instruction */
    token = trimStartEnd(token);
    i = is_inst(token);
    if(i >= 0){ /*only setting the type if a legal instruction was found*/
        (*ast).typeofLine = inst;
        assign_ast_dir_inst(ast,inst,i);
    }
    else if(i == -1){
        strcpy((*ast).errors, "Illegal instruction\n");
        return 0;
    }

    /*initializing the offset and register numbers for both operands*/
    for(j = 0; j < 2; j++){
        (*ast).operands.inst_ops[j].data_inst.offset.label = NULL;
        (*ast).operands.inst_ops[j].data_inst.offset.num = -1;
        (*ast).operands.inst_ops[j].reg_num = -1;
        (*ast).operands.inst_ops[j].address_of_op = none;
    }
    
    /* checking the operands of the instruction*/
    if(!check_inst_operands(ast, line, i, saveptr)) return 0; 

    return 1;
}

int check_inst_operands(frontend_ast *ast, char *line,int opcodeNum, char **saveptr){
    
    char *token,*token_ind,delim[2],token_backup[MAX_LINE_LEN];
    int index,check_reg,i; /*index indicates the number of legal operands scanned*/
    char *check_label;
    offset check_offset;
    address_0_op operand_type_0; /**/
    address_type type_of_address;

    /* checking the case of no operands */
    if(!op_codes[opcodeNum].arg_num && !isEmptyString(line)){
        strcpy((*ast).errors, "Too many arguments for instructions rts,hlt - those demand no operands\n");
        return 0;
    }
    /* assigning the right delimiter for case of 2 or 1 operands */
    if(op_codes[opcodeNum].arg_num == 2){ /*2 operands*/
        strcpy(delim, ",");
        index = 0;
    } 
    else if(op_codes[opcodeNum].arg_num == 1){ /*1 operand*/
        strcpy(delim, " ");
        index = 1;
    }
    
    token = my_strtok(line, delim, saveptr);
    i = 0;
    token_ind = line;
    while(op_codes[opcodeNum].arg_num && token){
        
        /*saving the backup token to use for investigating commas*/
        strcpy(token_backup,token);

        token = trimStartEnd(token);
        if(isEmptyString(token)){
            /*if at the start we have an empty token - missing first operand*/
            if((op_codes[opcodeNum].arg_num == 2 && !index) || (op_codes[opcodeNum].arg_num == 1 && index == 1)){
                strcpy((*ast).errors, "missing operand for instruction\n");
            }
            else if(op_codes[opcodeNum].arg_num == 2){
                /*no first operand - there's a comma after*/
                if(i == 0 && strchr(token_ind,',')) strcpy((*ast).errors, "Missing first argument before comma instruction\n");
                /*if there's a comma before and after and no operand*/
                else if(i > 0 && strchr(token_ind,','))  strcpy((*ast).errors, "Consecutive commas\n");
                /*more than 2 arguments seprated by a comma*/
                else if(i > 0 && !strchr(token_ind,',') && index > 1) strcpy((*ast).errors, "Too many arguments for instruction\n");
                /*missing 2nd argument after comma*/
                else if(i > 0 && !strchr(token_ind,',')) strcpy((*ast).errors, "Missing argument after comma\n");
            }
            return 0;
        }
        /*if there are more operands than permitted*/
        if(index > 1){
            strcpy((*ast).errors, "Too many arguments for instruction\n");
            return 0;
        }

        /*checking the possible types of operands*/
        operand_type_0 = address_type_0(ast, token,saveptr);
        check_label = check_legal_label(ast, token, 1, saveptr);
        check_offset = check_label_offset(ast, token);
        check_reg = is_reg(token);
        
        
        /*address none means it's not a legal operand for the # operand*/
        if(operand_type_0.type != none_0){ 
            /*case of const number and the subcases that follow*/
            if(operand_type_0.type == num){
                (*ast).operands.inst_ops[index].address_of_op = const_num;
                (*ast).operands.inst_ops[index].data_inst.type_data = int_data;
                (*ast).operands.inst_ops[index++].data_inst.data_option.num = operand_type_0.option.num;
            }
            if(operand_type_0.type == label_0){
                (*ast).operands.inst_ops[index].address_of_op = const_num;
                (*ast).operands.inst_ops[index].data_inst.type_data = label_data;
                (*ast).operands.inst_ops[index++].data_inst.data_option.label = operand_type_0.option.label;
            }
            if(operand_type_0.type == label_offset_0){
                (*ast).operands.inst_ops[index].address_of_op = const_num;
                (*ast).operands.inst_ops[index].data_inst.type_data = label_data;
                (*ast).operands.inst_ops[index].data_inst.data_option.label = operand_type_0.option.label_offset.label_array;
                (*ast).operands.inst_ops[index++].data_inst.offset = operand_type_0.option.label_offset.offset_val;
            }
        }
        /*if the operand is a label - no offset*/
        else if(check_label){
            (*ast).operands.inst_ops[index].address_of_op = label;
            (*ast).operands.inst_ops[index].data_inst.type_data = label_data;
            (*ast).operands.inst_ops[index++].data_inst.data_option.label = check_label;
        }
        /*if the operand is a label with an offset*/
        else if(check_offset.label_array){
            (*ast).operands.inst_ops[index].address_of_op = label_offset;
            (*ast).operands.inst_ops[index].data_inst.type_data = label_data;
            (*ast).operands.inst_ops[index].data_inst.data_option.label = check_offset.label_array;
            (*ast).operands.inst_ops[index++].data_inst.offset = check_offset.offset_val;
        }
        /*if the operand is a register*/
        else if(check_reg >= 0){
            (*ast).operands.inst_ops[index].address_of_op = reg;
            (*ast).operands.inst_ops[index++].reg_num = check_reg;
        }
        /*if the operand is none of the permitted type*/
        if(operand_type_0.type == none_0 && !check_label && !check_offset.label_array && check_reg == -1){
            strcpy((*ast).errors, "Illegal operand for instruction\n");
            return 0;
        }
        
        token = my_strtok(NULL, delim, saveptr);
        if(++i > 0 && strchr(token_ind,',')) token_ind += (int)strlen(token_backup) + 1;
    } /* end while loop*/

    /* if index is lower than 2 - we didn't scan enough operands*/
    if(op_codes[opcodeNum].arg_num == 2 && index < 2){
        strcpy((*ast).errors, "Too few arguments for instruction\n");
        return 0;
    }

    /* checking possiblity of address types for certain operations */
    /*2 operand options*/
    if(opcodeNum == 0 || opcodeNum == 2 || opcodeNum == 3 || opcodeNum == 6){
        type_of_address = (*ast).operands.inst_ops[1].address_of_op;
        if(type_of_address == const_num){
            strcpy((*ast).errors, "For operations mov,add,sub,lea the second operand cannot be a constant number\n");
            return 0;
        }
        if(opcodeNum == 6){
            type_of_address = (*ast).operands.inst_ops[0].address_of_op;
            if(type_of_address == const_num || type_of_address == reg){
                strcpy((*ast).errors, "For operation lea the first operand cannot be a constant number or a register\n");
                return 0;
            }
        }
    }

    /*one operand options*/
    if(opcodeNum == 4 || opcodeNum == 5 || (opcodeNum >= 7 && opcodeNum <= 11) || opcodeNum == 13){
        type_of_address = (*ast).operands.inst_ops[1].address_of_op;
        if(type_of_address == const_num){
            strcpy((*ast).errors, "For operations not,clr,inc,dec,jmp,bne,red,jsr the operand cannot be a constant number\n");
            return 0;
        }
    }
    if(opcodeNum == 9 || opcodeNum == 10 || opcodeNum == 13){
        type_of_address = (*ast).operands.inst_ops[1].address_of_op;
        if(type_of_address == label_offset){
            strcpy((*ast).errors, "For operations jmp,bne,jsr the operand cannot be a label with an offset\n");
            return 0;
        }
    }

    return 1;
}

address_0_op address_type_0(frontend_ast *ast,char *str, char **saveptr){

    address_0_op *result; /*the struct that contains all 3 options for const number operand*/
    char *check_label;
    offset *check_offset;

    result = (address_0_op*) calloc(1,sizeof(address_0_op));
    check_offset = (offset*) calloc(1,sizeof(offset));
    if(!result || !check_offset){
        strcpy((*ast).errors, "Memory allocation failed\n");
        return *result;
    }

    (*result).type = none_0;
    str = trimStartEnd(str);
    /* the character # defines address type 0 - if it's not there,it's 
    not const number operand*/
    if(!strchr(str,'#') || str[0] != '#') return (*result);

    str++; /*if after the # we have space, obviously it's illegal DUHHH*/
    if(isspace(str[0])){
        strcpy((*ast).errors, "Illegal operand of address type 0 in instruction\n");
        return (*result);
    }

    /*the 3 options - just an integer,label,label with offset*/
    *check_offset = check_label_offset(ast,str);
    if(is_integer(str,inst)){ /*integer*/
        (*result).type = num;
        (*result).option.num = atoi(str);
    }
    /*regular label*/
    else if((check_label = check_legal_label(ast,str,1,saveptr))){
        (*result).type = label_0;
        (*result).option.label = check_label;
    }
    /*label with offset*/
    else if(((*check_offset).label_array)){
        (*result).type = label_offset_0;
        (*result).option.label_offset.label_array = (*check_offset).label_array;
        (*result).option.label_offset.offset_val = (*check_offset).offset_val;
    } 
    else{ /*if the type is none of the 3 options*/
        (*result).type = none_0;
        return (*result);
    }

    return *result;
}

/************************************************************************************/
/************************************************************************************/
/****************************Auxiliary functions*************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/

int is_reg(char* str){

    int  i;

    /* Defining the registers used by the fictional machine - for effective comparison*/
    char *registers[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

    /* If the recieved string is null, there's nothing to do here*/
    if(!str) return -2;

    /* If we encounter a match with a register*/
    for(i = 0; i < REG_NUM; i++){
        if(strcmp(str, registers[i]) == 0){
            return i;
        }
    }

    /* arriving here - no match with any register was found */
    return -1;
}

int is_inst(char* str){
    int i;

    /* If the recieved string is null, there's nothing to do here*/
    if(!str) return -2;

    /*checking a match with the op_codes array - 
    matching the instruction codes*/
    for(i = 0; i < OPCODE_NUM; i++){
        if(strcmp(str, op_codes[i].opcode) == 0) return i;
    }

    return -1;
}

int is_dir(char* str){

    int i;

    /* If the recieved string is null, there's nothing to do here*/
    if(!str) return -2;

    /* Going through the directives in the dir_list array
    and looking for a match*/
    for (i = 0; i < DIR_NUM; i++){
        if(strcmp(str, dir_list[i]) == 0) return i;
    }

    return -1;
}

void assign_ast_dir_inst(frontend_ast *ast,line_type type,int i){

    /*just assigning the type of operation to the AST - and 
    the operation's name*/
    if(type == inst){
        switch (i){
        case 0:
            (*ast).operation_code.inst_code = op_mov;
            break;
        case 1:
            (*ast).operation_code.inst_code = op_cmp;
            break;
        case 2:
            (*ast).operation_code.inst_code = op_add;
            break;
        case 3:
            (*ast).operation_code.inst_code = op_sub;
            break;
        case 4:
            (*ast).operation_code.inst_code = op_not;
            break;
        case 5:
            (*ast).operation_code.inst_code = op_clr;
            break;
        case 6:
            (*ast).operation_code.inst_code = op_lea;
            break;
        case 7:
            (*ast).operation_code.inst_code = op_inc;
            break;
        case 8:
            (*ast).operation_code.inst_code = op_dec;
            break;
        case 9:
            (*ast).operation_code.inst_code = op_jmp;
            break;
        case 10:
            (*ast).operation_code.inst_code = op_bne;
            break;
        case 11:
            (*ast).operation_code.inst_code = op_red;
            break;
        case 12:
            (*ast).operation_code.inst_code = op_prn;
            break;
        case 13:
            (*ast).operation_code.inst_code = op_jsr;
            break;
        case 14:
            (*ast).operation_code.inst_code = op_rts;
            break;
        case 15:
            (*ast).operation_code.inst_code = op_hlt;
            break;
        }
    }
    else if(type == dir){
        switch (i)
        {
        case 0:
            (*ast).operation_code.dir_code = dir_entry;
            break;
        case 1:
            (*ast).operation_code.dir_code = dir_extern;
            break;
        case 2:
            (*ast).operation_code.dir_code = dir_data;
            break;
        case 3:
            (*ast).operation_code.dir_code = dir_string;
            break;
        case 4:
            (*ast).operation_code.dir_code = dir_define;
            break;
        }
    }
}

offset check_label_offset(frontend_ast *ast, char *str){
    offset *offset_var;
    char *start_offset,*end_offset,*label_offset,*num_offset,*token,**saveptr;
    int i,is_num_offset,num;

    /*allocating memory for the offset struct*/
    offset_var = (offset*) calloc(1,sizeof(offset));
    saveptr = (char**) calloc(1,sizeof(char*));/*to use in tokenization*/
    if(!saveptr || !offset_var){
        strcpy((*ast).errors, "Memory allocation failed\n");
        return *offset_var;
    }

    /*initializing the fields - offset of -1 is not an array offset*/
    (*offset_var).label_array = NULL;
    (*offset_var).offset_val.num = -1;
    /*finding the opening and closing brackets*/
    start_offset = strchr(str,'[');
    end_offset = strchr(str,']');
    /* four conditions:
    1 - either one of the characters [ ] is missing 
    2 - we have something like ] [ which is not how offset works
    3 - the string is [some...] which is illegal label basically
    4 - the string has more than 1 of each bracket - something like [[]
    */
    if((!(start_offset || end_offset)) || (start_offset > end_offset) \
    || (str == start_offset) || (!check_brackets(str,'['))){ /* no offset */
        return *offset_var;
    }
    /*finding anything between closing and opening brackets*/
    token = my_strtok(str,"[]",saveptr);
    i = 1;
    
    while(token){
        if(i == 1){ /* label of the offset*/
            if(!(label_offset = check_legal_label(ast,token,1,saveptr)) \
            || (strlen(token) > strlen(label_offset))) return *offset_var;
        }
        else if(i == 2){ /* the offset number itself  */
            is_num_offset = is_integer(token,inst); /*just an integer*/
            num_offset = check_legal_label(ast,token,1,saveptr);
            if(is_num_offset) num = atoi(token);
            if(!is_num_offset && !num_offset) return *offset_var; 
        }
        /* there are more than 2 tokens in the string meaning something
        after the closing brackets*/
        else if(i > 2 && token && !isEmptyString(token)) return *offset_var;

        token = my_strtok(NULL,"[]",saveptr);
        i++;
    }

    /*saving the legal label into the offset struct*/
    (*offset_var).label_array = (char*) calloc(strlen(label_offset) + 1,sizeof(char));
    if(!(*offset_var).label_array){
        strcpy((*ast).errors, "Memory allocation failed\n");
        return *offset_var;
    }
    strcpy((*offset_var).label_array,label_offset);

    /*saving the number into the offset struct - either as label or as an integer*/
    if(is_num_offset){
        (*offset_var).offset_val.num = num; /*just an integer*/
        /*if the number representing offset is larger than the maximmal possible,
        we have a problem, since the maximal amount of elements in a .data 
        directive is constrained by the maximal line length*/
        if(((*offset_var).offset_val.num < 0 || (*offset_var).offset_val.num > MAX_OFFSET)){
            strcpy((*ast).errors, "Value of offset in label[offset] is out of range for possible offset values \n");
        }
    }/*if the number is a label - we save it as a label*/
    else (*offset_var).offset_val.label = num_offset;

    free(saveptr);
    return *offset_var;
}

char* trimStartEnd(char* str){
    char* copy = NULL;
    int i,len;

    copy = (char*) calloc(strlen(str) + 1, sizeof(char));
    if(copy == NULL){ /* allocation failed */
        return NULL;
    }

    strcpy(copy,str);
    /* trimming white spaces from the beggining */
    i = 0;
    while (isspace(str[i])){
        copy++;
        i++;
    }
    /* trimming white spaces from the end */
    len = strlen(copy);
    i = len - 1;
    while (isspace(copy[i])) i--;

    copy[i+1] = '\0';
    
    return copy;
}

char* my_strtok(char *str, const char *delim,char **saveptr){
    char *copy_str;
    char *str_copy = NULL; 

    /* this is the first interation of tokenization - happens only once*/
    if (str){
        copy_str = (char*) calloc(strlen(str) + 1,sizeof(char));
        if(!copy_str){
            printf("Memory allocation for str_copyizing failed\n");
            return NULL;
        }
        strcpy(copy_str,str);
        *(saveptr) = copy_str;
    }

    if (!(*saveptr)) return NULL; /* the final iteration - reaching the end */

    /* remember current start as found str_copy */
    str_copy = (*saveptr);

    /* find next occurrence of delim */
    (*saveptr) = strpbrk((*saveptr), delim);
    /* replace delim with terminator and move start to follower */
    if ((*saveptr)) *(*saveptr)++ = '\0';
    /* done */
    return str_copy;
}

int isEmptyString(char* str){
    /*trimming just to be sure. if even after trimming it's 
    empty - it was just white spaces*/
    str = trimStartEnd(str); 
    if(strcmp(str,"") == 0) return 1;
    return 0;
}

void frontend_init(frontend_ast *ast){
    
    (*ast).typeofLine = empty;
    (*ast).errors = (char*) calloc(INIT_ERROR_LEN, sizeof(char));
    if((*ast).errors == NULL){ /* failed memory allocation*/
        fprintf(stdout, "Memory allocation failed\n");
        (*ast).typeofLine = error;
        exit(1);
    }
    (*ast).label[0] = '\0'; /*initialize the line label as empty string*/

}

void frontend_free(frontend_ast *ast){
    int i;

    /*freeing only if the pointers are each not null*/
    if((*ast).errors) free((*ast).errors);
    if((*ast).typeofLine == inst){
        if((*ast).operands.inst_ops[0].data_inst.offset.label) free((*ast).operands.inst_ops[0].data_inst.offset.label);
        if((*ast).operands.inst_ops[1].data_inst.offset.label) free((*ast).operands.inst_ops[1].data_inst.offset.label);
    }
    if((*ast).typeofLine == dir){
        for(i = 0; i < 2*MAX_LINE_LEN; i++){
            if((*ast).operands.dir_ops.data_dir[i].offset.label) free((*ast).operands.dir_ops.data_dir[i].offset.label);
        }
    }
}
    

int is_integer(char* str,line_type type){
    int i,num,len;

    str = trimStartEnd(str);
    if(!str || isEmptyString(str)) return 0;

    len = strlen(str);
    for(i = 0; i < len; i++){
        /*digits with plus/minus sign only at the start and without decimal
        point*/
        if((!isdigit(str[i]) && str[i] != '-' && str[i] != '+') \
         || (i > 0 && (str[i] == '-' || str[i] == '+')) || str[i] == '.'){
            return 0;
        }
    }
    num = atoi(str); /*if the number is legal, save it to check it's value*/
    /* the number is out of range of accepted values */
    if((type == inst && (num > MAX_INT_INST || num < MIN_INT_INST)) || \
    (type == dir && (num > MAX_INT_DIR || num < MIN_INT_DIR))){
        return 0;
    }

    return 1;
}

int check_brackets(char *str,char c){

    int i,len,open,close,string_bracket;

    len = strlen(str);
    open = close = string_bracket = 0;

    for(i = 0; i < len; i++){
        /* counting opening square brackets*/
        if(c == '[' && str[i] == '[')      open++;
        /* counting closing square brackets*/
        else if(c == '[' && str[i] == ']') close++;
        /*counting double quotes for use in string directives*/
        else if(c == '\"' && str[i] == '\"')      string_bracket++;
    }
    /*legal situation is either having more than 2 double quotes 
    or having [] one opening square bracket and one closing*/
    if((c == '\"' && string_bracket > 1) || (c == '[' && open == 1 && close == 1)){
        return 1;
    }

    return 0;
}