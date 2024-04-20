#include "../Header_Files/ast.h"
#include "../Header_Files/global_var.h"

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

frontend_ast *frontend(char* line){

    frontend_ast *ast; 
    char *copy_line,*char_skip,*label;
    int check_dir,check_inst,check_empty;

    /* Allocate memory for the AST */
    ast = (frontend_ast *) calloc(1, sizeof(frontend_ast));
    if(ast == NULL){
        fprintf(stderr, "Error: Memory allocation failed for the AST structure\n");
        return ast;
    }

    /* initialize the AST */
    frontend_init(ast);

    /*If length of the line exceeds the maximal length - we skip that line*/
    if(strlen(line) > MAX_LINE_LEN){
        strcpy(ast->errors, "Assembly line too long - maximal length is 82 characters\n");
        ast->typeofLine = error;
        print_ast(ast,line);
        return ast;
    }

    /*creating a copy of the line for printing purposes or for removing newline characters*/
    copy_line = (char*) calloc(strlen(line) + 1, sizeof(char));
    if(copy_line == NULL){
        strcpy(ast->errors, "Error: Memory allocation failed for copy of the assembly line string\n");
        ast->typeofLine = error;
        return ast;
    }
    
    /* removing the newline character from the end of the line*/ 
    if(line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
    
    strcpy(copy_line,line);

    /* a case where a newline character is somewhere in the middle - cutting the line off*/
    if(strchr(line, '\n') && line[strlen(line) - 1] != '\n' && check_mid_newline(line)){
        strcpy(ast->errors, "a newline character appears in the middle of the line\n");
        ast->typeofLine = error;
        print_ast(ast,copy_line);
        free(copy_line);
        return ast;
    } 

    
    /* found ; at the start of the line signifying it's a possible comment line*/
    /* Or if the line is just empty - only white spaces */
    check_empty = isEmptyString(trimStartEnd(line));
    if(line[0] == ';' || check_empty){
        ast->errors[0] = '\0';
        ast->typeofLine = empty;
        print_ast(ast,copy_line);
        free(copy_line);
        return ast;
    }
    else if(strchr(line,';') && line[0] != ';'){
        strcpy(ast->errors, "the character ; appears illegally in the middle of the line\n");
        ast->typeofLine = error;
        print_ast(ast,copy_line);
        free(copy_line);
        return ast;
    }

    /* trimming the line from extra spaces at the start and at the end - since those 
    do not contribute to anything, save for a migrane*/
    line = trimStartEnd(line);

    /* found : signifying there's a possible label at the start of the line*/
    label = NULL;
    if((char_skip = strchr(line, ':'))){
        if(!(label = check_legal_label(ast, line, 0))){ /*checking the label*/
            if(ast->errors[0] == '\0') strcpy(ast->errors, "Illegal declaration of label\n");
            ast->typeofLine = error;
            ast->label[0] = '\0';
            free(copy_line);
            return ast;
        }
        else strcpy(ast->label, label);
    }

    /* If a label was found, we move past it to analyze the rest of the line*/
    if(char_skip && label) line = char_skip + 1;
    line = trimStartEnd(line); /*again trimming start and end white space to isolate the operation*/

    /* found . signifying it's a possible directive line */
    if((char_skip = strchr(line, '.')) && find_dir(line)){
        check_dir = check_directive(ast, line); /*checking legallity of directive*/
        if(!check_dir){
            ast->typeofLine = error;
            print_ast(ast,copy_line);
            free(copy_line);
            return ast;
        }
    }
    /* found no . meaning there's only one other possible option - either
    it's an instruction or another migrane */
    else{
        check_inst = check_instruction(ast, line);
        if(!check_inst){
            ast->typeofLine = error;
            print_ast(ast,copy_line);
            free(copy_line);
            return ast;
        }
    }

    /*If we encountered no errors - then set the ast.errors field to NULL*/
    if(ast->typeofLine != error || isEmptyString(ast->errors)) ast->errors[0] = '\0';

    /*print ast for debug*/
    print_ast(ast,copy_line);

    if(label) free(label);
    free(copy_line);

    return ast;
}


char *check_legal_label(frontend_ast *ast, char *str,int arg){

    char *token,*label = NULL;
    int i,error_flag; /*error_flag - 1 means error exists. 0 means not*/
    /*saveptr - to serve as the starting point in tokenation
    movptr - saves the string and draws out the tokens in in each iteration*/
    char **saveptr = NULL,movptr[MAX_LINE_LEN];

    
    error_flag = 0;
    /* extracting the part of the label, before a colon - if it's a label at the start of the line*/
    if(!arg){
        saveptr = create_saveptr(saveptr);
        if(!saveptr) return NULL;
        *saveptr = movptr;
        memset(movptr,0,MAX_LINE_LEN);

        token = my_strtok(str, ":", saveptr);
        if(isEmptyString(token)){ /* if nothing is before the colon*/
            strcpy(ast->errors, "Missing label declaration before colon\n");
            error_flag = 1;
        } /* if between the colon and the label there's a space*/
        else if(isspace(token[strlen(token) - 1])){
            strcpy(ast->errors, "declaration of label must be adjacent to colon without spaces\n");
            error_flag = 1;
        }
        if(error_flag){
            FREE_SAVEPTR(saveptr,movptr)
            return NULL;
        } 
    } 
    else token = str; /*if not - simply check the string that it's a legal label*/
    token = trimStartEnd(token);

    
    /* checking errors - if the string is not a system reserved word */
    if(is_reg(token) >= 0 || is_inst(token) >= 0 || is_dir(token) >= 0 || \
    !strcmp(token, "mcr") || !strcmp(token, "endmcr")){
        if (!arg) strcpy(ast->errors, "A system reserved word (like mov , data , etc...) cannot be used as a label.\n");
        FREE_SAVEPTR(saveptr,movptr)
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

    /*saving the label if it was found without errors*/
    if(!error_flag){
        label = copystr_calloc(ast,label,token);
        if(!label){
            FREE_SAVEPTR(saveptr,movptr)
            return NULL;
        }
    } 
    else label = NULL;

    if(!arg){ /*if it's a line label (before a colon), checking what is after that*/
        token = my_strtok(NULL, ":", saveptr);
        /*if there's no space after the colon it's an error*/
        if(token && !isspace(token[0])){ 
            strcpy(ast->errors, "Missing space after label declaration\n");
            if(label) free(label);
            FREE_SAVEPTR(saveptr,movptr)
            return NULL;
        }
        /*if there's nothing after the colon, it's a label on an empty line which is illegal*/
        if(!token || isEmptyString(trimStartEnd(token))){
            strcpy(ast->errors, "Label cannot be declared on empty line\n");
            if(label) free(label);
            FREE_SAVEPTR(saveptr,movptr)
            return NULL;
        }    
    }

    FREE_SAVEPTR(saveptr,movptr)
    
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

int check_directive(frontend_ast *ast, char *line){

    char *token;
    int i,j,check_args;
    /*saveptr - to serve as the starting point in tokenation
    movptr - saves the string and draws out the tokens in tokenation*/
    char **saveptr = NULL,movptr[MAX_LINE_LEN];

    saveptr = create_saveptr(saveptr);
    if(!saveptr) return 0;
    *saveptr = movptr;
    memset(movptr,0,MAX_LINE_LEN); /* setting the array to zero*/


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
        if(i == 4) ast->typeofLine = define; /*since define is in on it's own*/
        else ast->typeofLine = dir;
        assign_ast_dir_inst(ast,dir,i);
    }
    else if(i == -1){
        strcpy(ast->errors, "Illegal directive\n");
        FREE_SAVEPTR(saveptr,movptr)
        return 0;
    }

    /*initializing offset values for all the arguments*/
    for(j = 0; j < 2*MAX_LINE_LEN; j++){
        DIR_OP_DATA(ast,j).offset.label = NULL;
        DIR_OP_DATA(ast,j).offset.num = -1;
    }

    /* Refering to the relevant function to check the operands of the directives*/
    switch (ast->operation_code.dir_code){   
    case dir_entry:
    case dir_extern:
        check_args = check_entry_extern(ast, line);
        break;
    case dir_data:
        check_args = check_data_dir(ast, line);
        break;
    case dir_string:
        check_args = check_string(ast, line);
        break;
    case dir_define:
        check_args = check_define(ast, line);
        break;
    }

    if(!check_args){
        ast->typeofLine = error;
        FREE_SAVEPTR(saveptr,movptr)
        return 0;
    }

    FREE_SAVEPTR(saveptr,movptr)
    return 1;
}

int check_entry_extern(frontend_ast *ast, char *line){
    
    char *token, *check_label;
    int index;
    /*saveptr - to serve as the starting point in tokenation
    movptr - saves the string and draws out the tokens in tokenation*/
    char **saveptr = NULL,movptr[MAX_LINE_LEN];

    saveptr = create_saveptr(saveptr);
    if(!saveptr) return 0;
    *saveptr = movptr;
    memset(movptr,0,MAX_LINE_LEN); /* setting the array to zero*/


    
    token = my_strtok(line, " ", saveptr);
    /* if there's nothing in the recieved string, then we have no operand */
    if(!token || isEmptyString(token)){
        strcpy(ast->errors, "Missing label after entry/extern directive\n");
        free(saveptr);
        return 0;
    }

    token = trimStartEnd(token);
    /*checking the argument label*/
    check_label = check_legal_label(ast, token, 1);
    if(!check_label){
        if(!FOUND_ALLOC_ERROR) strcpy(ast->errors, "Illegal label after entry/extern directive\n");
        FREE_SAVEPTR(saveptr,movptr)
        return 0;
    }

    index = ast->operands.dir_ops.num_count;
    /*inserting the label into the ast*/
    DIR_OP_DATA(ast,index).type_data = label_data;
    DIR_OP_DATA(ast,index).data_option.label = copystr_calloc(ast,DIR_OP_DATA(ast,index).data_option.label,check_label);
    if(!DIR_OP_DATA(ast,index).data_option.label){
        free(check_label);
        FREE_SAVEPTR(saveptr,movptr)
        return 0;
    } 

    ast->operands.dir_ops.num_count = ++index;

    /* extra arguments after the label */
    if((token = my_strtok(NULL, " ", saveptr))){
        strcpy(ast->errors, "Too many arguments for directive entry/extern\n");
        free(check_label);
        FREE_SAVEPTR(saveptr,movptr)
        return 0;
    }

    /*if we have a line label - it is meaningless for entry/extern so
    we delete it to prevent confusion*/
    if(!isEmptyString(ast->label)) ast->label[0] = '\0';
    
    free(check_label);
    FREE_SAVEPTR(saveptr,movptr)
    
    return 1;
}

int check_data_dir(frontend_ast *ast, char *line){

    char *token,*token_ind,*is_label = NULL;
    char token_backup[MAX_LINE_LEN],*delim = ",";
    int is_int,index,i;
    offset *is_label_offset;
    /*saveptr - to serve as the starting point in tokenation
    movptr - saves the string and draws out the tokens in tokenation*/
    char **saveptr = NULL,movptr[MAX_LINE_LEN];

    saveptr = create_saveptr(saveptr);
    if(!saveptr) return 0;
    *saveptr = movptr;
    memset(movptr,0,MAX_LINE_LEN); /* setting the array to zero*/

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
            if(i == 0 && !strchr(token_ind,',')) strcpy(ast->errors, "Missing arguments for .data directive\n");
            /*no first operand - there's a comma after*/
            if(i == 0 && strchr(token_ind,',')) strcpy(ast->errors, "Missing first argument before comma for .data directive\n");
            /*if there's a comma before and after and no operand*/
            else if(i > 0 && strchr(token_ind,','))  strcpy(ast->errors, "Consecutive commas\n");
            /*if there's a comma before, no comma after and no operand*/
            else if(i > 0 && !strchr(token_ind,',')) strcpy(ast->errors, "Missing argument after comma\n");
            return 0;
        }
        /*the options for each operand*/
        is_int = is_integer(token,dir);/*just an integer*/
        /*a label defined as integer elsewhere*/
        is_label = check_legal_label(ast,token,1);
        if(!is_label && FOUND_ALLOC_ERROR){
            FREE_SAVEPTR(saveptr,movptr)
            return 0;
        } 
        /*an integer represented by an expression label[something] - 
        meaning represented by the result of a data array offset*/
        is_label_offset = check_label_offset(ast,token);
        if(!is_label_offset){
            if(is_label) free(is_label);
            FREE_SAVEPTR(saveptr,movptr)
            return 0;
        }

        /*if the operand matched none of the 3 allowed types*/
        if(!is_int && !is_label && !is_label_offset->label_array){ /* token is not a variable name or a legal integer - either not a number or the integer is out of bounds */
            strcpy(ast->errors, "Illegal data argument\n");
            free_offset_struct(is_label_offset);
            FREE_SAVEPTR(saveptr,movptr)
            return 0;
        }

        /*if the operand's legal - save it into the AST */
        index = ast->operands.dir_ops.num_count;
        if(is_int){ /*integer*/
            DIR_OP_DATA(ast,index).type_data = int_data;
            DIR_OP_DATA(ast,index++).data_option.num = atoi(token);
        } 
        else if(is_label){ /*integer represented by a label*/
            DIR_OP_DATA(ast,index).type_data = label_data;
            DIR_OP_DATA(ast,index).data_option.label = copystr_calloc(ast,DIR_OP_DATA(ast,index).data_option.label,is_label);
            if(!DIR_OP_DATA(ast,index).data_option.label){
                free(is_label);
                if(is_label_offset) free_offset_struct(is_label_offset);
                FREE_SAVEPTR(saveptr,movptr)
                return 0;
            } 
            index++;
        }
        else if(is_label_offset->label_array){ /*offset value representation*/
            DIR_OP_DATA(ast,index).type_data = label_data;
            /*if we have label[offset] -- copying the 'label' part*/
            DIR_OP_DATA(ast,index).data_option.label = copystr_calloc(ast,DIR_OP_DATA(ast,index).data_option.label , is_label_offset->label_array);
            if(!DIR_OP_DATA(ast,index).data_option.label){
                if(is_label) free(is_label);
                free_offset_struct(is_label_offset);
                FREE_SAVEPTR(saveptr,movptr)
                return 0;
            } 

            if(is_label_offset->offset_val.label){
                /*if we have label[offset] -- copying the 'offset' part*/
                DIR_OP_DATA(ast,index).offset.label = copystr_calloc(ast,DIR_OP_DATA(ast,index).offset.label , is_label_offset->offset_val.label);
                if(!DIR_OP_DATA(ast,index).offset.label){
                    if(is_label) free(is_label);
                    free_offset_struct(is_label_offset);
                    FREE_SAVEPTR(saveptr,movptr)
                    return 0;
                }
                index++;
            }
            else if(is_label_offset->offset_val.num != -1){
                DIR_OP_DATA(ast,index++).offset.num = is_label_offset->offset_val.num;
            }
               
        } /* end if(is_label_offset->label_array)*/
        
        /*increasing the total number of operands between iterations*/
        ast->operands.dir_ops.num_count = index;
        
        /*advancing the token_ind pointer to be at the start of the next token*/
        if(++i > 0 && strchr(token_ind,',')) token_ind += (int)strlen(token_backup) + 1;
        token = my_strtok(NULL, delim, saveptr);
    
        /*freeing the offset structure*/
        if(is_label) free(is_label);
        free_offset_struct(is_label_offset);

    } /* end while loop*/

    FREE_SAVEPTR(saveptr,movptr)
    return 1;
}

int check_string(frontend_ast *ast, char *line){
    char *start,*end;
    int i,j,index,len;

    /*if the operand is empty*/
    if(!line || isEmptyString(line)){
        strcpy(ast->errors, "Missing string argument\n");
        return 0;
    }
    /*if there's less the 2 sets of double quotes*/
    if(!strchr(line,'\"') || !check_brackets(line,'\"')){
        strcpy(ast->errors, "String declared in .string directive must be enclosed in double quotes\n");
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
            strcpy(ast->errors, "string in .string directive not enclosed in double quotes\n");
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
        strcpy(ast->errors, "Illegal string in .string directive\n");
        return 0;
    }

    /*saving the string into the AST*/
    index = ast->operands.dir_ops.num_count;

    DIR_OP_DATA(ast,index).type_data = string;

    DIR_OP_DATA(ast,index).data_option.string = copystr_calloc(ast,DIR_OP_DATA(ast,index).data_option.string,start);
    if(!DIR_OP_DATA(ast,index).data_option.string) return 0;

    /*saving only part between the first and last double quotes into the string in the AST*/
    memcpy(DIR_OP_DATA(ast,index).data_option.string ,start,end - start);
    DIR_OP_DATA(ast,index++).data_option.string[end - start] = '\0';

    ast->operands.dir_ops.num_count = index;

    return 1;
}

int check_define(frontend_ast *ast, char *line){

    char *token,*label;
    char *delim = "=";
    int index,num,check_num;
    /*saveptr - to serve as the starting point in tokenation
    movptr - saves the string and draws out the tokens in tokenation*/
    char **saveptr = NULL,movptr[MAX_LINE_LEN];

    saveptr = create_saveptr(saveptr);
    if(!saveptr) return 0;
    *saveptr = movptr;
    memset(movptr,0,MAX_LINE_LEN); /* setting the array to zero*/

    /* if a legal line label was found before a define line of .define directive - it's an error*/
    if(!isEmptyString(ast->label)){
        strcpy(ast->errors, "A label cannot be declared on a line with .define directive\n");
        FREE_SAVEPTR(saveptr,movptr)
        return 0;
    }

    /* first token - the symbol*/
    token = my_strtok(line, delim, saveptr);
    token = trimStartEnd(token);

    if(!token || isEmptyString(token)){ /*empty token - no symbol*/
        strcpy(ast->errors, "Missing symbol after .define directive\n");
        FREE_SAVEPTR(saveptr,movptr)
        return 0;
    }

    label = NULL; /*checking the legality of the symbol - like a label*/
    if(!(label = check_legal_label(ast, token, 1))){
        FREE_SAVEPTR(saveptr,movptr)
        if(!FOUND_ALLOC_ERROR){
            strcpy(ast->errors, "Illegal symbol after .define directive\n");
            return 0;
        } 
        else return 0;
    }

    /* second token - the number assigned to the symbol */
    token = my_strtok(NULL, delim, saveptr);
    token = trimStartEnd(token);

    if(!token || isEmptyString(token)){ /*no number after symbol*/
        strcpy(ast->errors, "Missing integer in .define directive\n");
        if(label) free(label);
        FREE_SAVEPTR(saveptr,movptr)
        return 0;
    }

    if(!(check_num = is_integer(token,dir))){ /*if the number operand is illegal*/
        strcpy(ast->errors, "Illegal operand after = .define directive - supposed to be an integer\n");
        if(label) free(label);
        FREE_SAVEPTR(saveptr,movptr)
        return 0;
    }
    else num = atoi(token); /*saving the number*/

    /*only if both operands are legal, we save them into the AST*/
    if(label && check_num){ 
        index = ast->operands.dir_ops.num_count;

        /* the first operand - the variable name*/
        DIR_OP_DATA(ast,index).  type_data = label_data;
        DIR_OP_DATA(ast,index).data_option.label = copystr_calloc(ast,DIR_OP_DATA(ast,index).data_option.label,label);
        if(!DIR_OP_DATA(ast,index).data_option.label){
            free(label);
            FREE_SAVEPTR(saveptr,movptr)
            return 0;
        }
        index++;
        /* the second operand - the number*/
        DIR_OP_DATA(ast,index).  type_data = int_data;
        DIR_OP_DATA(ast,index++).data_option.num = num;
        /**/
        ast->operands.dir_ops.num_count = index;
    }

    if(label) free(label);
    FREE_SAVEPTR(saveptr,movptr)
    return 1;
}

/**************************************************************************************/
/**************************************************************************************/
/****************************Functions for instructions********************************/
/**************************************************************************************/
/**************************************************************************************/


int check_instruction(frontend_ast *ast, char *line){
    char *token;
    int i,j;
    /*saveptr - to serve as the starting point in tokenation
    movptr - saves the string and draws out the tokens in tokenation*/
    char **saveptr = NULL,movptr[MAX_LINE_LEN];

    saveptr = create_saveptr(saveptr);
    if(!saveptr) return 0;
    *saveptr = movptr;
    memset(movptr,0,MAX_LINE_LEN); /* setting the array to zero*/

    /*first token - the instruction operation code (mov,add, etc...)*/
    token = my_strtok(line, " ", saveptr);
    line += strlen(token) + 1; /* skipping the instruction for checking operands later */
    line = trimStartEnd(line);
    
    i = -1; /*initializing value to check type of operation*/
    /* Identifying the instruction */
    token = trimStartEnd(token);
    i = is_inst(token);
    if(i >= 0){ /*only setting the type if a legal instruction was found*/
        ast->typeofLine = inst;
        assign_ast_dir_inst(ast,inst,i);
    }
    else if(i == -1){
        strcpy(ast->errors, "Illegal instruction\n");
        FREE_SAVEPTR(saveptr,movptr)
        return 0;
    }

    /*initializing the offset and register numbers for both operands*/
    for(j = 0; j < 2; j++){
        INST_OP_DATA(ast,j).offset.label = NULL;
        INST_OP_DATA(ast,j).offset.num = -1;
        ast->operands.inst_ops[j].reg_num = -1;
        ast->operands.inst_ops[j].address_of_op = none;
    }
    
    /* checking the operands of the instruction*/
    if(!check_inst_operands(ast, line, i)){
        FREE_SAVEPTR(saveptr,movptr)
        return 0;
    }  

    FREE_SAVEPTR(saveptr,movptr)
    return 1;
}

int check_inst_operands(frontend_ast *ast, char *line,int opcodeNum){
    
    char *token,*token_ind,delim[2],token_backup[MAX_LINE_LEN];
    int index,check_reg,i; /*index indicates the number of legal operands scanned*/
    char *check_label;
    offset *check_offset = NULL;
    address_0_op *operand_type_0; 
    address_type type_of_address;
    /*saveptr - to serve as the starting point in tokenation
    movptr - saves the string and draws out the tokens in tokenation*/
    char **saveptr = NULL,movptr[MAX_LINE_LEN];


    /* checking the case of no operands and the line
    is not empty - meaning there's extra text*/
    if(!op_codes[opcodeNum].arg_num && !isEmptyString(line)){
        strcpy(ast->errors, "Extranous text after instructions rts,hlt - those have no operands!\n");
        return 0;
    }

    /* assigning the right delimiter for case of 2 or 1 operands */
    /*2 operands*/
    if(op_codes[opcodeNum].arg_num == 2) index = 0;
    /*1 operand*/
    else if(op_codes[opcodeNum].arg_num == 1) index = 1;
    strcpy(delim, ",");
    token = NULL;

    saveptr = create_saveptr(saveptr);
    if(!saveptr) return 0;
    *saveptr = movptr;
    memset(movptr,0,MAX_LINE_LEN); /* setting the array to zero*/

    if(op_codes[opcodeNum].arg_num != 0){
        token = my_strtok(line, delim, saveptr);
        i = 0;
        token_ind = line;
    }
    
    while(op_codes[opcodeNum].arg_num && token){
        
        /*saving the backup token to use for investigating commas*/
        strcpy(token_backup,token);

        token = trimStartEnd(token);
        if(isEmptyString(token)){
            /*if at the start we have an empty token - missing first operand*/
            if((op_codes[opcodeNum].arg_num == 2 && !index) || (op_codes[opcodeNum].arg_num == 1 && index == 1)){
                strcpy(ast->errors, "missing operand for instruction\n");
            }
            else if(op_codes[opcodeNum].arg_num == 2){
                /*no first operand - there's a comma after*/
                if(i == 0 && strchr(token_ind,',')) strcpy(ast->errors, "Missing first argument before comma instruction\n");
                /*if there's a comma before and after and no operand*/
                else if(i > 0 && strchr(token_ind,','))  strcpy(ast->errors, "Consecutive commas\n");
                /*more than 2 arguments seprated by a comma*/
                else if(i > 0 && !strchr(token_ind,',') && index > 1) strcpy(ast->errors, "Too many arguments for instruction\n");
                /*missing 2nd argument after comma*/
                else if(i > 0 && !strchr(token_ind,',')) strcpy(ast->errors, "Missing argument after comma\n");
            }
            FREE_SAVEPTR(saveptr,movptr)
            return 0;
        }
        /*if there are more operands than permitted*/
        if(index > 1){
            strcpy(ast->errors, "Too many arguments for instruction\n");
            FREE_SAVEPTR(saveptr,movptr)
            return 0;
        }

        /*checking the possible types of operands*/
        check_label = check_legal_label(ast, token, 1);
        if(!check_label && FOUND_ALLOC_ERROR){
            FREE_SAVEPTR(saveptr,movptr)
            return 0;
        }
        check_offset = check_label_offset(ast, token);
        if(check_offset == NULL){
            if(check_label) free(check_label);
            FREE_SAVEPTR(saveptr,movptr)
            return 0;
        }
        operand_type_0 = address_type_0(ast, token);
        if(operand_type_0 == NULL){ /*meaning allocation failed*/
            if(check_label) free(check_label);
            free_offset_struct(check_offset);
            FREE_SAVEPTR(saveptr,movptr)
            return 0;
        }  
        check_reg = is_reg(token);


        /*address none means it's not a legal operand for the # operand*/
        if(operand_type_0->type != none_0){

            ast->operands.inst_ops[index].address_of_op = const_num;
            /*case of const number and the subcases that follow*/
            if(operand_type_0->type == num){ /* the operand is just an integer*/
                INST_OP_DATA(ast,index).type_data = int_data;
                INST_OP_DATA(ast,index++).data_option.num = operand_type_0->option.num;
            }
            /*an integer represented by a label*/
            else if(operand_type_0->type == label_0){
                INST_OP_DATA(ast,index).type_data = label_data;
                
                INST_OP_DATA(ast,index).data_option.label = \
                copystr_calloc(ast,INST_OP_DATA(ast,index).data_option.label,operand_type_0->option.label);
                if(!INST_OP_DATA(ast,index).data_option.label){
                    if(check_label) free(check_label);
                    if(check_offset) free_offset_struct(check_offset);
                    free_op_address(operand_type_0);
                    return 0;
                } 
                
                index++;
            }
            /*an integer represented by label[offset] - a number reference by an
            offset of another data set*/
            else if(operand_type_0->type == label_offset_0){
                INST_OP_DATA(ast,index).type_data = label_data;
                
                INST_OP_DATA(ast,index).data_option.label = \
                copystr_calloc(ast,INST_OP_DATA(ast,index).data_option.label ,operand_type_0->option.label_offset.label_array);
                if(!INST_OP_DATA(ast,index).data_option.label){
                    if(check_label) free(check_label);
                    if(check_offset) free_offset_struct(check_offset);
                    free_op_address(operand_type_0);
                    return 0;
                } 
                
                if(operand_type_0->option.label_offset.offset_val.label){
                    INST_OP_DATA(ast,index).offset.label = copystr_calloc(ast,INST_OP_DATA(ast,index).offset.label,operand_type_0->option.label_offset.offset_val.label);
                    if(!INST_OP_DATA(ast,index).offset.label){
                        if(check_label) free(check_label);
                        if(check_offset) free_offset_struct(check_offset);
                        free_op_address(operand_type_0);
                        return 0;
                    } 
                    index++;
                }
                else if(operand_type_0->option.label_offset.offset_val.num >= 0){
                    INST_OP_DATA(ast,index++).offset.num = operand_type_0->option.label_offset.offset_val.num;
                }
            }
        }
        /*if the operand is a label - no offset*/
        else if(check_label){
            ast->operands.inst_ops[index].address_of_op = label;
            INST_OP_DATA(ast,index).type_data = label_data;
            INST_OP_DATA(ast,index).data_option.label = copystr_calloc(ast,INST_OP_DATA(ast,index).data_option.label,check_label);
            if(!INST_OP_DATA(ast,index).data_option.label){
                free(check_label);
                free_offset_struct(check_offset);
                free_op_address(operand_type_0);
                return 0;
            }
            index++;
        }
        /*if the operand is a label with an offset*/
        else if(check_offset->label_array){

            ast->operands.inst_ops[index].address_of_op = label_offset;
            INST_OP_DATA(ast,index).type_data = label_data;
            /*copying the label of the array of the offset into the AST*/
            INST_OP_DATA(ast,index).data_option.label = copystr_calloc(ast,INST_OP_DATA(ast,index).data_option.label , check_offset->label_array);
            if(!INST_OP_DATA(ast,index).data_option.label){
                if(check_label) free(check_label);
                free_offset_struct(check_offset);
                free_op_address(operand_type_0);
                return 0;
            }

            if(check_offset->offset_val.label){
                INST_OP_DATA(ast,index).offset.label = copystr_calloc(ast,INST_OP_DATA(ast,index).offset.label , check_offset->offset_val.label);
                if(!INST_OP_DATA(ast,index).offset.label){
                    if(check_label) free(check_label);
                    free_offset_struct(check_offset);
                    free_op_address(operand_type_0);
                    return 0;
                }
                index++;
            }
            else if(check_offset->offset_val.num >= 0){
                INST_OP_DATA(ast,index++).offset.num = check_offset->offset_val.num;
            }
        }
        /*if the operand is a register*/
        else if(check_reg >= 0){
            ast->operands.inst_ops[index].address_of_op = reg;
            ast->operands.inst_ops[index++].reg_num = check_reg;
        }
        /*if the operand is none of the permitted type*/
        if(operand_type_0->type == none_0 && !check_label && !check_offset->label_array && check_reg == -1){
            strcpy(ast->errors, "Illegal operand for instruction\n");
            free_offset_struct(check_offset);
            free_op_address(operand_type_0);
            FREE_SAVEPTR(saveptr,movptr)
            return 0;
        }
        
        
        if(check_label) free(check_label);

        /*freeing the address_0_op variable - and it's subcomponents if those were dynamically allocated*/
        free_op_address(operand_type_0);

        /*freeing the offset variable and it's subcomponents*/
        free_offset_struct(check_offset);
    
        if(++i > 0 && strchr(token_ind,',')) token_ind += (int)strlen(token_backup) + 1;
        token = my_strtok(NULL, delim, saveptr);
    } /* end while loop*/

    /* if index is lower than 2 - we didn't scan enough operands*/
    if(op_codes[opcodeNum].arg_num == 2 && index < 2){
        strcpy(ast->errors, "Too few arguments for instruction\n");
        return 0;
    }

    /* checking possiblity of address types for certain operations */
    /*2 operand options*/
    if(opcodeNum == 0 || opcodeNum == 2 || opcodeNum == 3 || opcodeNum == 6){
        type_of_address = ast->operands.inst_ops[1].address_of_op;
        if(type_of_address == const_num){
            strcpy(ast->errors, "For operations mov,add,sub,lea the second operand cannot be a constant number\n");
            return 0;
        }
        if(opcodeNum == 6){
            type_of_address = ast->operands.inst_ops[0].address_of_op;
            if(type_of_address == const_num || type_of_address == reg){
                strcpy(ast->errors, "For operation lea the first operand cannot be a constant number or a register\n");
                return 0;
            }
        }
    }

    /*one operand options*/
    if(opcodeNum == 4 || opcodeNum == 5 || (opcodeNum >= 7 && opcodeNum <= 11) || opcodeNum == 13){
        type_of_address = ast->operands.inst_ops[1].address_of_op;
        if(type_of_address == const_num){
            strcpy(ast->errors, "For operations not,clr,inc,dec,jmp,bne,red,jsr the operand cannot be a constant number\n");
            return 0;
        }
    }
    if(opcodeNum == 9 || opcodeNum == 10 || opcodeNum == 13){
        type_of_address = ast->operands.inst_ops[1].address_of_op;
        if(type_of_address == label_offset){
            strcpy(ast->errors, "For operations jmp,bne,jsr the operand cannot be a label with an offset\n");
            return 0;
        }
    }

    FREE_SAVEPTR(saveptr,movptr)
    return 1;
}


/************************************************************************************/
/************************************************************************************/
/****************************Auxiliary functions*************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/

offset *check_label_offset(frontend_ast *ast, char *str){
    offset *offset_var = NULL;
    /*label_array - the name of the array that defines the offset
    label_offset  - the name of the offset value inside if it's defined by a label*/
    char *start_offset,*end_offset,*label_array,*label_offset,*token;
    int i,is_offset_int,num;
    char token_backup[MAX_LINE_LEN];
    /*saveptr - to serve as the starting point in tokenation
    movptr - saves the string and draws out the tokens in tokenation*/
    char **saveptr = NULL,movptr[MAX_LINE_LEN];

   
    /*allocating memory for the offset struct*/
    offset_var = (offset*) calloc(1,sizeof(offset));
    if(!offset_var){
        strcpy(ast->errors, "Memory allocation failed for offset variable\n");
        return offset_var;
    }

    /*initializing the fields - offset of -1 is not an array offset*/
    offset_var->offset_val.num = -1;
    offset_var->offset_val.label = NULL;
    offset_var->label_array = NULL;

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
        return offset_var;
    }

    saveptr = create_saveptr(saveptr);
    if(!saveptr){
        free_offset_struct(offset_var);
        return NULL;
    }
    *saveptr = movptr;
    memset(movptr,0,MAX_LINE_LEN); /* setting the array to zero*/

    /*finding anything between closing and opening brackets*/
    token = my_strtok(str,"[]",saveptr);
    i = 1;
    
    while(token){
        /*saving the backup token to properly spaces between the label_array and the []*/
        strcpy(token_backup,token);

        if(i == 1){ /* label of the offset*/
            label_array = check_legal_label(ast,token,1);
            /*allocation failed for the label_array for the offset*/
            if(!label_array && FOUND_ALLOC_ERROR){
                FREE_SAVEPTR(saveptr,movptr)
                free_offset_struct(offset_var);
                return NULL;
            }
            /*allocation successful but it's not a legal label*/
            if(!label_array || (strlen(token_backup) > strlen(label_array))){
                if(label_array) free(label_array);
                FREE_SAVEPTR(saveptr,movptr)
                return offset_var;
            } 
        }
        else if(i == 2){ /* the offset number itself  */
            is_offset_int = is_integer(token,inst); /*just an integer*/
            label_offset = check_legal_label(ast,token,1); /*integer presented as a variable*/
            if(!label_offset && FOUND_ALLOC_ERROR){
                if(label_array) free(label_array);
                free_offset_struct(offset_var);
                FREE_SAVEPTR(saveptr,movptr)
                return NULL;
            }
            if(is_offset_int) num = atoi(token);

            /*if the offset is not a label or integer - NOT AN OFFSET*/
            if(!is_offset_int && !label_offset){
                if(label_array) free(label_array);
                FREE_SAVEPTR(saveptr,movptr)
                return offset_var; 
            }
        }
        /* there are more than 2 tokens in the string meaning something
        after the closing brackets - it's not an offset*/
        else if(i > 2 && token && !isEmptyString(token)){
            FREE_SAVEPTR(saveptr,movptr)
            if(label_offset) free(label_offset);
            if(label_array) free(label_array);
            return offset_var;
        }

        token = my_strtok(NULL,"[]",saveptr);
        i++;
    }

    /*saving the legal label into the offset struct*/
    offset_var->label_array = copystr_calloc(ast,offset_var->label_array,label_array);
    if(!offset_var->label_array){
        if(label_offset) free(label_offset);
        if(label_array) free(label_array);
        free_offset_struct(offset_var);
        FREE_SAVEPTR(saveptr,movptr)
        return NULL;
    } 

    /*saving the number into the offset struct - either as label or as an integer*/
    if(is_offset_int){
        /*if the number representing offset is larger than the maximmal possible,
        we have a problem, since the maximal amount of elements in a .data 
        directive is constrained by the maximal line length*/
        if(num < 0 || num > MAX_OFFSET){
            strcpy(ast->errors, "Value of offset in label[offset] is out of range for possible offset values \n");
            if(label_offset) free(label_offset);
            if(label_array) free(label_array);
            if(offset_var->label_array) free(offset_var->label_array);
            offset_var->label_array = NULL;
            FREE_SAVEPTR(saveptr,movptr)
            return offset_var;
        }
        /*saving the number if it's within bounds*/
        offset_var->offset_val.num = num; /*just an integer*/

        
    }/*if the number is a label - we save it as a label*/
    else if(label_offset){
        offset_var->offset_val.label = copystr_calloc(ast,offset_var->offset_val.label,label_offset);
        if(!offset_var->offset_val.label){
            if(label_array) free(label_array);
            if(label_offset) free(label_offset);
            free_offset_struct(offset_var);
            FREE_SAVEPTR(saveptr,movptr)
            return NULL;
        } 
    } 

    if(label_array) free(label_array);
    if(label_offset) free(label_offset);
    FREE_SAVEPTR(saveptr,movptr)
    return offset_var;
}

void free_aux_structs(address_0_op *operand_type_0,offset *offset_var,char *label){
    if(label) free(label);
    if(offset_var != NULL){
        if(offset_var->label_array) free(offset_var->label_array);
        if(offset_var->offset_val.label) free(offset_var->offset_val.label);
        free(offset_var);
    }
    if(operand_type_0 != NULL){
        if(operand_type_0->type == label_0)
            free(operand_type_0->option.label);    
        
        if(operand_type_0->type == label_offset_0){
            if(operand_type_0->option.label_offset.label_array) 
                free(operand_type_0->option.label_offset.label_array);
            if(operand_type_0->option.label_offset.offset_val.label)
                free(operand_type_0->option.label_offset.offset_val.label);
        }
            
        free(operand_type_0);
    }  
    
}

void free_offset_struct(offset *offset_var){
    if(offset_var->label_array) free(offset_var->label_array);
    if(offset_var->offset_val.label) free(offset_var->offset_val.label);
    free(offset_var);
}

void free_op_address(address_0_op *operand_type_0){
    if(operand_type_0->type == label_0)
        free(operand_type_0->option.label);    
    
    else if(operand_type_0->type == label_offset_0){
        if(operand_type_0->option.label_offset.label_array) 
            free(operand_type_0->option.label_offset.label_array);
        if(operand_type_0->option.label_offset.offset_val.label)
            free(operand_type_0->option.label_offset.offset_val.label);
    }
        
    free(operand_type_0);
}

address_0_op *address_type_0(frontend_ast *ast,char *str){

    address_0_op *result = NULL; /*the struct that contains all 3 options for const number operand*/
    char *check_label;
    offset *check_offset = NULL;
    

    result = (address_0_op*) calloc(1,sizeof(address_0_op));
    if(!result){
        strcpy(ast->errors, "Memory allocation error\n");
        return result;
    }

    /*setting the default type for address type zero - if that's changed
    then the operand is of address type zero - in the format #something*/
    result->type = none_0;
    str = trimStartEnd(str);
    /* the character # defines address type 0 - if it's not there,it's 
    not const number operand*/
    if(!strchr(str,'#') || str[0] != '#') return result;

    str++; /*if after the # we have space, obviously it's illegal DUHHH*/
    if(isspace(str[0])){
        strcpy(ast->errors, "Illegal operand of address type 0 in instruction\n");
        return result;
    }

    /*the 3 options - just an integer,label,label with offset*/
    check_label = check_legal_label(ast,str,1);
    if(!check_label && FOUND_ALLOC_ERROR){
        free(result);
        return NULL;
    }
    check_offset = check_label_offset(ast,str);
    if(check_offset == NULL){
        if(check_label) free(check_label);
        free(result);
        return NULL;
    }

    /*if the operand is an integer*/
    if(is_integer(str,inst)){ 
        result->type = num;
        result->option.num = atoi(str);
    }
    /*regular label*/
    else if(check_label){
        result->type = label_0;
        result->option.label = copystr_calloc(ast,result->option.label , check_label);
        if(!result->option.label){
            free(check_label);
            free_offset_struct(check_offset);
            free(result);
            return NULL;
        }
        
    }
    /*label with offset*/
    else if(check_offset->label_array){
        result->type = label_offset_0;
        /*allocating the label of the array for the offset*/
        result->option.label_offset.label_array = copystr_calloc(ast,result->option.label_offset.label_array , check_offset->label_array);
        if(!result->option.label_offset.label_array){
            if(check_label) free(check_label);
            free_offset_struct(check_offset);
            if(result->option.label) free(result->option.label);
            free(result);
            return NULL;
        } 
        /*if the offset value is a label, allocate memory for it*/
        if(check_offset->offset_val.label){
            result->option.label_offset.offset_val.label = copystr_calloc(ast,result->option.label_offset.offset_val.label , check_offset->offset_val.label);
            if(!result->option.label_offset.offset_val.label){
                if(check_label) free(check_label);
                free_offset_struct(check_offset);
                if(result->option.label) free(result->option.label);
                free(result->option.label_offset.label_array);
                free(result);
                return NULL;
            }
        }
        else if(check_offset->offset_val.num >= 0){
            result->option.label_offset.offset_val.num = check_offset->offset_val.num;
        }
        
    } 
    else{ /*if the type is none of the 3 options*/
        result->type = none_0;
        if(check_label) free(check_label);
        free_offset_struct(check_offset);
        return result;
    }
 
    if(check_label) free(check_label);
    free_offset_struct(check_offset);
    return result;
}

char *copystr_calloc(frontend_ast *ast, char* dest, const char *src){
    dest = (char *) calloc(strlen(src) + 1,sizeof(char));
    if(!dest){
        strcpy(ast->errors, "Error: Memory allocation failed for label/string variable in AST\n");
        return NULL;
    }
    strcpy(dest,src);
    return dest;
}

char **create_saveptr(char **saveptr){
    saveptr = (char **) calloc(1,sizeof(char *));
    if(!saveptr){
        printf("Error: Memory allocation failed for *saveptr.\n");
        return NULL;
    }
    return saveptr;
}

int check_mid_newline(char *line){
    char *token,*token1;
    int i;
    /*saveptr - to serve as the starting point in tokenation
    movptr - saves the string and draws out the tokens in tokenation*/
    char **saveptr = NULL,movptr[MAX_LINE_LEN];

    saveptr = create_saveptr(saveptr);
    if(!saveptr) return 0;
    *saveptr = movptr;
    memset(movptr,0,MAX_LINE_LEN); /* setting the array to zero*/

    i = 0;
    token = my_strtok(line, "\n", saveptr);
    token1 = token;
    
    while(token){
        /* only if there's something (not white spaces) before and after the first newline character*/
        if(i > 0 && !isEmptyString(token1) && !isEmptyString(token)) return 1;
        token = my_strtok(NULL, "\n", saveptr);
        i++;
    }

    FREE_SAVEPTR(saveptr,movptr)
    return 0;
}

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

    int j;
    /*just assigning the type of operation to the AST - and 
    the operation's name*/
    /*in case it's an instruction*/
    for(j = 0; (type == inst && j < OPCODE_NUM) || \
    ((type == dir || type == define) && j < DIR_NUM); j++){
        if(i == j){
            if(type == inst){
                ast->operation_code.inst_code = (inst_opcode) j;
            } 
            else if(type == dir){
                ast->operation_code.dir_code = (dir_opcode) j;
            }
            break;
        }
    } /* end for loop*/
}


char *trimStartEnd(char* str){
    char *end;
    
    /* trimming white spaces from the beggining */
    while (isspace(*str)) str++;
    
    /* trimming white spaces from the end */
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';

    return str;
}


char* my_strtok(char *str, const char *delim,char **saveptr){
 
    char *str_copy; 

    /* this is the first interation of tokenization - happens only once*/
    if (str) strcpy((*saveptr),str);

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
    /*trimming just in case - to be absolutely sure*/
    str = trimStartEnd(str);
    if(strcmp(str,"") == 0) return 1;
    
    return 0;
}

void frontend_init(frontend_ast *ast){
    
    ast->typeofLine = empty;
    ast->errors[0] = '\0'; /*initialize the errors string as empty string*/
    ast->label[0] = '\0'; /*initialize the line label as empty string*/

}

void frontend_free(frontend_ast *ast){
    int i;

    if(ast->typeofLine == inst){

        for(i = 0; i < 2; i++){

            if(INST_OP_DATA(ast,i).type_data == label_data){
                if(INST_OP_DATA(ast,i).offset.label){
                    free(INST_OP_DATA(ast,i).offset.label);
                }
                free(INST_OP_DATA(ast,i).data_option.label);
            }
            if(INST_OP_DATA(ast,i).type_data == string && INST_OP_DATA(ast,i).data_option.string){
                free(INST_OP_DATA(ast,i).data_option.string);
            }
            
        } /* end for loop over intruction operands */
        
    }

    else if(ast->typeofLine == dir || ast->typeofLine == define){
        
        for(i = 0; i < ast->operands.dir_ops.num_count; i++){
            /*if we had a label allocated - designated as label data*/
            if(DIR_OP_DATA(ast,i).type_data == label_data && DIR_OP_DATA(ast,i).data_option.label){ 
                free(DIR_OP_DATA(ast,i).data_option.label);
            } 
            /*if we have an offset as a label*/
            if(DIR_OP_DATA(ast,i).offset.label) free(DIR_OP_DATA(ast,i).offset.label);
            
            /* string in .string directive*/
            if(DIR_OP_DATA(ast,i).type_data == string && DIR_OP_DATA(ast,i).data_option.string){
                free(DIR_OP_DATA(ast,i).data_option.string);
            }
        } /* end for loop over directive operands*/
    }
    free(ast);
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