/*directive can have STRING[2] or STRING[len] but not STRING[LOOP[2]]*/

#include "../Header_Files/vector_lib.h"
#include "../Header_Files/firstSecond_Pass.h" 
#include "../Header_Files/global_var.h"
#include "../Header_Files/t_unit.h"
#include "../Header_Files/lexer.h"

/* part of adding new symbol struct to vector*/
void * symbol_ctor (const void * copy) {
    /*allocate memory for a new symbol structure*/
    struct symbol * new_p = (struct symbol *) malloc(sizeof(struct symbol));
    if(new_p == NULL) 
        return NULL;    
    /*copy the contents of the provided symbol*/
    return memcpy(new_p,copy,sizeof(struct symbol));
} 
/*delete function for vector  */
void symbol_dtor(void * item){
    free(item);
}

void * extern_ctor (const void * copy) {
    /*allocate memory for a new extern structure*/
    struct ext * new_p = (struct ext *) malloc(sizeof(struct ext));
    if(new_p == NULL) 
        return NULL; 
    /*copy the contents of the provided extern*/
    return memcpy(new_p,copy,sizeof(struct ext));
}
/*delete function for vector  */
void extern_dtor(void * item){
    free(item);
}

struct symbol * init_symbol(struct symbol *symbol){
    symbol = (struct symbol *) malloc(sizeof(struct symbol));
    if(symbol == NULL){
        return symbol;
    }
    symbol->address = 0;
    strcpy(symbol->symName,"\0");
    symbol->len_data = 0;
    symbol->len_string = 0;
    symbol->is_symbol_define = FALSE;
    symbol->define_val = MAX_OFFSET + 1; /*make sure the number is initialized as illegal - out of bounds*/
    symbol->num_line_defined = -1;
    return symbol;
}

struct symbol * does_symbol_exist(struct vector * symbol_vector, char * name){
    struct symbol * symbol_pointer;
    void *const *begin;
    void *const *end;
    /*iterate through the symbols vec*/
    VECTOR_LOOP(begin, end, symbol_vector) {
        if(*begin){
            symbol_pointer = (struct symbol *) *begin;
            /*if the symbol name matches the provided name*/
            if(strcmp(symbol_pointer->symName, name) == 0){
                return symbol_pointer;
            }
        } 
    }
    return NULL;
}


struct ext * does_extern_exist(struct vector * extern_vector, char * name){
    struct ext * extern_pointer;
    void *const *begin;
    void *const *end;
    /*iterate through the symbols vec*/
    VECTOR_LOOP(begin, end, extern_vector) {
        if(*begin) {
            extern_pointer = (struct ext *) *begin;
            /*if the symbol name matches the provided name*/
            if(strcmp(extern_pointer->ext_name, name) == 0){
                return extern_pointer;
            }
        } 
    }
    return NULL;
}


/***********************************************************************************************************************/

int firstPass(struct translation_unit *translation_unit, char *amFileName, FILE *amFile) {
    int i,len;
    char *line = NULL;
    int ic = IC_START;
    int dc = 0;
    int is_error = FALSE;
    int line_counter = 1;

    ast_tree *ast;
    dir_opcode dir_type; 
    struct symbol *find_symbol;
    void *const *begin;
    void *const *end;
    char *string1,c;

    struct symbol *symbol = NULL;
       

    while ((c = fgetc(amFile)) != EOF) {
        
        symbol = init_symbol(symbol);
        if (symbol == NULL) {
            printf("********** error: Memory allocation error\n");
            free_translation_unit(translation_unit);
            exit(1);
        }
        
        ast = NULL;

        is_error = FALSE;
        line = read_line_input(c,amFile);/*read line from file*/
        ast = ast_line(line);/*get the ast struct for each line*/

        /*if AST found errors, print them and move to the next line*/
        /*Or if the line is empty, move on to the next line*/
        if (ast->errors[0] != '\0' || ast->typeofLine == empty) {
            if(ast->errors[0] != '\0'){
                if(ALLOC_ERR_LEXER){
                    printf("**********  error: Memory allocation error\n");
                    free_translation_unit(translation_unit);
                    free_ast(ast);
                    free(symbol);
                    free(line);
                    exit(1);
                    
                }
                
                printf("********** error: syntax error in file: %s line: %s\n    %s\n", amFileName, line, ast->errors);
                is_error = TRUE;
            }
            line_counter++;
            free_ast(ast);
            free(symbol);
            free(line);
            continue;
        }
       
        /*check if their is a label in the line and if label is of code line or data\string type*/
        if (strlen(ast->label_of_line) > 0 && (ast->typeofLine == inst ||
            (ast->typeofLine == dir && (ast->operation_code.dir_code == dir_data || ast->operation_code.dir_code == dir_string)))) {
            /*check to see if label all ready exists*/
            find_symbol = does_symbol_exist(translation_unit->symbols, ast->label_of_line);
            if (find_symbol) { /*is symbol exists*/
                /*change type of symbol accordingly*/
                if (find_symbol->symType == entrySymbol) {
                    if (ast->typeofLine == inst) {
                        find_symbol->symType = entryCodeSymbol;
                        find_symbol->address = ic;
                    } else {
                        find_symbol->symType = entryDataSymbol;
                        find_symbol->address = dc;
                        /*saving the size of the string and data arrays*/
                        if(ast->operation_code.dir_code == dir_string){
                        find_symbol->len_string = (int) strlen(DIR_OP_DATA(ast,0).data_option.string);
                        }
                        else if(ast->operation_code.dir_code == dir_data){
                            find_symbol->len_data = ast->operands.dir_ops.num_count;
                        }
                    }
                    translation_unit->entry_use = TRUE;
                } else {/*if symbol all ready exists and not of type entry then it has to be a redefinition error*/
                    printf("********** error: in file %s line %d: %s\n    redefinition of symbol %s\n", amFileName,line_counter, line, find_symbol->symName);
                    is_error = TRUE;
                }
            }else{/*if symbol doesn't all ready exist, we will add it to symbol vector*/

                strcpy(symbol->symName, ast->label_of_line);
                if (ast->typeofLine == inst) {
                    symbol->symType = codeSymbol;
                    symbol->address = ic;
                } else {
                    symbol->symType = dataSymbol;
                    symbol->address = dc;
                    /*saving the number of elements (characters in .string directive and numbers in .data directive)
                    to check for possible out of bounds error in said data arrays*/
                    if(ast->operation_code.dir_code == dir_string){
                        symbol->len_string = (int) strlen(DIR_OP_DATA(ast,0).data_option.string);
                    }
                    else if(ast->operation_code.dir_code == dir_data){
                        symbol->len_data = ast->operands.dir_ops.num_count;
                    }
                }
                vector_insert(translation_unit->symbols, symbol);
            }
        }

        /*if line is of instruction type*/
        /*memory word - a cell of size 14 bits */
        if(ast->typeofLine == inst) {
                /*each instruction counts as a memory word*/
                ic++;
                /*both instruction operands are registers, then there's another memory word for the registers*/
                if(ast->operands.inst_ops[0].reg_num != -1 && ast->operands.inst_ops[1].reg_num != -1){
                    ic++;
                }
                else{
                    
                    for(i = 0; i<2;i++){
                        if(ast->operands.inst_ops[i].reg_num != -1){
                            ic++;
                        }
                        else if(ast->operands.inst_ops[i].address_of_op == none){
                            /*do not add to ic*/
                        }
                        else if(ast->operands.inst_ops[i].reg_num == -1){
                            ic++;
                            if(ast->operands.inst_ops[i].address_of_op == label_offset){
                                ic++;
                            }
                        }
                    }
                }
                /*else, if line is of type directive, we will update dc accordingly */
        }
        else if(ast->typeofLine == dir && (ast->operation_code.dir_code == dir_data || ast->operation_code.dir_code == dir_string )) {
            if(DIR_OP_DATA(ast,0).type_data == int_data || DIR_OP_DATA(ast,0).type_data == label_data){
                dc += ast->operands.dir_ops.num_count;
            }
            else if(DIR_OP_DATA(ast,0).type_data == string){
                dc += strlen(DIR_OP_DATA(ast,0).data_option.string) +1;
            }
        }

        /*incase line is of type extern or entry*/
        dir_type = ast->operation_code.dir_code;
        if (ast->typeofLine == dir && ((dir_type == dir_entry) || (dir_type == dir_extern))) {
            /*find if symbol all ready exists*/
            find_symbol = does_symbol_exist(translation_unit->symbols, DIR_OP_DATA(ast,0).data_option.label);
            if ((find_symbol != NULL) && (dir_type == dir_entry)) {
                /*if symbol found update symbol type if symbol wasn't all ready defined as extern or entry,or used in .define directive*/
                if (find_symbol->symType == dataSymbol && find_symbol->is_symbol_define == FALSE) {
                    find_symbol->symType = entryDataSymbol;
                    translation_unit -> entry_use = TRUE;
                } else if (find_symbol->symType == codeSymbol) { /*instruction lines - add,mov , etc....*/
                    find_symbol->symType = entryCodeSymbol;
                    translation_unit -> entry_use = TRUE;
                } else {
                    /*if symbol is defined as entry or extern but all ready was defined as 
                    entry or extern then it has to be a error*/
                    if(find_symbol->symType == externSymbol){
                        printf("********** error: in file %s line %d: %s\n    illegal redefinition of entry label as extern: %s\n", amFileName,line_counter, line, find_symbol->symName);
                    }
                    else if(find_symbol->is_symbol_define == TRUE){
                        printf("********** error: in file %s line %d: %s\n    illegal redefinition of .define directive label as entry label: %s\n", amFileName,line_counter, line, find_symbol->symName);
                    }
                    else{
                        printf("********** error: in file %s line %d: %s\n    illegal redefinition of entry label as entry: %s\n", amFileName,line_counter, line, find_symbol->symName);
                    }
                    is_error = TRUE; /*change error flag to TRUE*/
                }
            } /*else, if symbol doesn't all ready exist, we will add it*/
            else if (!find_symbol) {
                strcpy(symbol->symName, DIR_OP_DATA(ast,0).data_option.label);
                symbol->symType = (symType) ast->operation_code.dir_code;
                vector_insert(translation_unit->symbols, symbol);
            } else {
                printf("********* label wasn't already defined\n");
                printf("********* Semantic error in file: %s line %d: %s\n    redefinition of symbol: %s\n", amFileName, line_counter, line, find_symbol->symName);
                is_error = TRUE;
            }
        /*if line is of type define */
        }else if(ast->typeofLine == define){
            /*check to see if symbol all ready exists*/
            find_symbol = does_symbol_exist(translation_unit->symbols,  DIR_OP_DATA(ast,0).data_option.label);
            if(find_symbol){
                /*is symbol all ready exists then it has to be a redefinition error*/
                if(find_symbol->is_symbol_define == TRUE){
                    printf("********* Semantic error in file: %s line %d: %s\n    symbol %s was already with .define directive \n", amFileName, line_counter, line, find_symbol->symName);
                }
                else{
                    printf("********* Semantic error in file: %s line %d: %s\n    symbol %s was already defined a label and can't be used in define directive \n", amFileName, line_counter, line, find_symbol->symName);
                }
            }
            /*else, wee will add symbol to symbols vector*/
            else{
                strcpy(symbol->symName, DIR_OP_DATA(ast,0).data_option.label);
                symbol->symType = dataSymbol;
                symbol->address = DIR_OP_DATA(ast,1).data_option.num;
                symbol->define_val = DIR_OP_DATA(ast,1).data_option.num;
                symbol->is_symbol_define = TRUE;
                symbol->num_line_defined = line_counter;
                vector_insert(translation_unit->symbols, symbol);
            }

        }
        line_counter++;     /*increase line counter each while loop*/
        free_ast(ast); /*free ast*/
        free(symbol);       /*free symbol*/
        free(line);         /*free line*/
        line = NULL;
    }

    /*checking to see if any entry symbol was not defined properly*/
    VECTOR_LOOP(begin, end, translation_unit->symbols) {
        if (*begin) {
            symbol = (struct symbol *) *begin;

            if (symbol->symType == entrySymbol && symbol->is_symbol_define == FALSE) {
                printf("********* error: in file: %s line %d: %s\n    symbol %s was declared as entry symbol but was not defined\n", amFileName, line_counter, line, symbol->symName);
                is_error = TRUE;
            }
            /*now that we know what ic is we can update the data symbols dc to the correct dc 
            by adding the ic to the dc*/
            if ((symbol->symType == dataSymbol || symbol->symType == entryDataSymbol) && (symbol->is_symbol_define == FALSE)) {
                symbol->address += ic;
            }
        }   
    }
  


    /*create data_image part*/
    rewind(amFile);
    line_counter = 1;
    /*iterate through the file again*/
    while ((c = fgetc(amFile)) != EOF) {
        
        ast = NULL;

        line = read_line_input(c,amFile);
        ast = ast_line(line); /*get the ast*/

         /*check if line is of type error*/

        if (ast->errors[0] != '\0') {
            line_counter++;
            free_ast(ast);
            free(line);
            continue;
        }
        
        /*incase of declaration data/string, update the DC accordingly */      
        if (ast->typeofLine == dir && (ast->operation_code.dir_code == dir_data || ast->operation_code.dir_code == dir_string)) {
            /*iterate number of times by length of string or amount of integers */
            for(i=0; i<ast->operands.dir_ops.num_count; i++){
                /*if data is integers*/
                if(DIR_OP_DATA(ast,i).type_data == int_data){
                    add_to_data_image(translation_unit,DIR_OP_DATA(ast,i).data_option.num);
                    dc++;
                    translation_unit->DC++;
                }
                /*if data is label data*/
                else if(DIR_OP_DATA(ast,i).type_data == label_data){
                    find_symbol = does_symbol_exist(translation_unit->symbols, DIR_OP_DATA(ast,i).data_option.label);
                    if(find_symbol){
                        if(find_symbol->is_symbol_define == TRUE){
                            if(line_counter < find_symbol->num_line_defined && find_symbol->num_line_defined != -1){
                                printf("********* error in file: %s line %d: %s\n    variable name %s used via .define directive must be defined before use: \n", amFileName, line_counter, line, find_symbol->symName);
                                is_error = TRUE;
                                continue;
                            }
                        }
                        add_to_data_image(translation_unit, find_symbol->address);
                        dc++;
                        translation_unit->DC++;/*add zero for end of string*/
                    }
                }
                /*if data is of string type*/
                else if(DIR_OP_DATA(ast,i).type_data == string){
                    string1 = DIR_OP_DATA(ast,i).data_option.string;
                    len = strlen(DIR_OP_DATA(ast,i).data_option.string);
                    for (i = 0; i < len; i++) {
                        add_to_data_image(translation_unit, (int)string1[i]);
                        dc++;
                        translation_unit->DC++;
                    }
                    add_to_data_image(translation_unit, 0);/*add zero for end of string*/
                    dc++;
                    translation_unit->DC++;
                }
                dc++;
            } 
        }
        line_counter++;
        free_ast(ast); /*free ast*/
        free(line);    /*free line*/
        line = NULL;
    }


    return is_error;
}




int secondPass(struct translation_unit *translation_unit, char *amFileName, FILE *amFile){
    int i, is_op_source,error_flag;
    char *line = NULL,c = '\0';
    int line_counter = 1;
    int is_error = FALSE;
    int data_arg_num,string_len;

    ast_tree *ast = NULL;
    struct ext *find_extern;
    struct symbol *find_symbol;

    struct ext *extern1 = (struct ext *) calloc(1,sizeof(struct ext));
    if(extern1 == NULL){
        printf("********* error: memory allocation error\n");
        free_translation_unit(translation_unit);
        exit(1);
    }


    rewind(amFile);
    is_error = FALSE;
    while ((c = fgetc(amFile)) != EOF) {
        
        error_flag = 0;
        line = read_line_input(c,amFile);
        ast = ast_line(line);
        
        /*empty line */
        if(ast->typeofLine == empty){
            free_ast(ast);
            free(line);
            line_counter++;
            continue;
        }
        is_op_source =0;

        /*in case that line is of type instruction */
        if(ast->typeofLine == inst){
            /*check for errors*/
            for(i=0; i<2; i++){
                if(ast->operands.inst_ops[i].address_of_op == label){
                    find_symbol = does_symbol_exist(translation_unit->symbols,INST_OP_DATA(ast,i).data_option.label);
                    if(!find_symbol){
                        printf("********* error in file: %s line %d: %s\n    symbol %s is used but was never defined\n", amFileName, line_counter, line, INST_OP_DATA(ast,i).data_option.label);
                        error_flag = 1;
                    }
                    else if(find_symbol->is_symbol_define == TRUE){
                        if(i == 0 && ast->operation_code.inst_code == op_lea){
                            printf("********* error in file: %s line %d: %s\n    first operand in instruction lea can not be of type define: %s\n", amFileName, line_counter, line, find_symbol->symName);
                        }
                        else if(i == 1 && ast->operation_code.inst_code != op_cmp && ast->operation_code.inst_code != op_prn){
                            printf("********* error in file: %s line %d: %s\n    second operand can not be a label which is defined as a number (via define): %s\n", amFileName, line_counter, line, find_symbol->symName);
                        }
                        if(line_counter < find_symbol->num_line_defined && find_symbol->num_line_defined != -1){
                            printf("********* error in file: %s line %d: %s\n    operand is a constant number (via define), but is not defined before first use: %s\n", amFileName, line_counter, line, find_symbol->symName);
                        }
                        error_flag = 1;
                    }
                }
            } /* end for loop */

            if(error_flag == 1){
                free_ast(ast);
                free(line);
                is_error = TRUE;
                line_counter++;
                continue;
            }
            /*first binary word, same format for all code line*/
            /*source operand, bits 4-5 */
            if(ast->operands.inst_ops[0].address_of_op == none && ast->operands.inst_ops[1].address_of_op == none){
                add_to_code_image(translation_unit,0);
            }
            if(ast->operands.inst_ops[0].address_of_op != none){
                add_to_code_image(translation_unit,ast->operands.inst_ops[0].address_of_op << 4);
            }
            else{
                is_op_source++;
            }
            /*destination operand, bits 2-3 */
            if(ast->operands.inst_ops[1].address_of_op != none && is_op_source == 0){
                translation_unit->code_image[translation_unit->IC] |= ast->operands.inst_ops[1].address_of_op << 2;
            }
            else if(ast->operation_code.inst_code != op_hlt && ast->operation_code.inst_code != op_rts){
                add_to_code_image(translation_unit,ast->operands.inst_ops[1].address_of_op << 2);
            }
            /*opcode, bits 6-9*/
            translation_unit->code_image[translation_unit->IC] |= ast->operation_code.inst_code << 6;
            translation_unit->IC++;

            /*check if of two register type*/
            if(ast->operands.inst_ops[0].reg_num != -1 && ast->operands.inst_ops[1].reg_num != -1){
                add_to_code_image(translation_unit,ast->operands.inst_ops[0].reg_num << 5);
                translation_unit->code_image[translation_unit->IC] |= ast->operands.inst_ops[1].reg_num << 2;
                translation_unit->IC++;
            }
            else{
                
                for(i=0; i<2; i++){
                    /*if first operand is reg*/
                    if(ast->operands.inst_ops[0].address_of_op == reg && i == 0){
                        add_to_code_image(translation_unit,ast->operands.inst_ops[0].reg_num << 5);
                        translation_unit->IC++;
                    }
                    /*if second operand is reg*/
                    if(ast->operands.inst_ops[1].address_of_op == reg && i == 1){
                        add_to_code_image(translation_unit,ast->operands.inst_ops[1].reg_num << 2);
                        translation_unit->IC++;
                    }  
                    if(ast->operands.inst_ops[i].address_of_op == label || ast->operands.inst_ops[i].address_of_op == label_offset ){
                        find_symbol = does_symbol_exist(translation_unit->symbols, INST_OP_DATA(ast,i).data_option.label);
                        if(find_symbol){
                            
                            if(find_symbol->symType != externSymbol){
                                add_to_code_image(translation_unit,find_symbol->address << 2);
                            }

                            if(find_symbol->symType == externSymbol){
                                /*extern case, ARE = 01*/
                                add_to_code_image(translation_unit,0);
                                translation_unit->code_image[translation_unit->IC] |= 1;
                                /* translation_unit->IC++; */
                                find_extern = does_extern_exist(translation_unit->externals,find_symbol->symName); 

                                /*if extern already exists in externals vec */
                                if(find_extern){
                                    find_extern->address[find_extern->address_count] = translation_unit->IC + IC_START;
                                    find_extern->address_count++;
                                    translation_unit->extern_use = TRUE;
                                }
                                /*if extern doesn't exist in externals vec, we will add it */
                                else{
                                    extern1->address[0] = translation_unit->IC + IC_START;
                                    extern1->address_count = 1;
                                    translation_unit->extern_use = TRUE;
                                    strcpy(extern1->ext_name, INST_OP_DATA(ast,i).data_option.label);
                                    vector_insert(translation_unit->externals, extern1); 
                                }                               
                            }/*if symbol is not extern*/
                            else{
                                translation_unit->code_image[translation_unit->IC] |= 2;
                            }
                            translation_unit->IC++;

                            /*incase that there is an offset*/
                            if(ast->operands.inst_ops[i].address_of_op == label_offset){
                                data_arg_num = find_symbol->len_data;
                                string_len = find_symbol->len_string;
                                /*checking errors - if the label (array name) is indeed assigned to .data or .string directive*/
                                if(!data_arg_num && !string_len){
                                    printf("********* error in file: %s line %d: %s\n    the label(array) %s which defines the offset must be assigned to a .data/.string directive line\n", amFileName, line_counter, line, find_symbol->symName);
                                    is_error = TRUE;
                                }

                                /*end checking errors*/

                                if(INST_OP_DATA(ast,i).offset.label != NULL){
                                    find_symbol = does_symbol_exist(translation_unit->symbols, INST_OP_DATA(ast,i).offset.label);
                                    if(find_symbol){
                                        if(find_symbol->is_symbol_define == FALSE){
                                            printf("********* error in file: %s line %d: %s\n    offset value %s must be defined using .define directive\n", amFileName, line_counter, line, find_symbol->symName);
                                            is_error = TRUE;
                                        }
                                        if((data_arg_num > 0 && (find_symbol->define_val > data_arg_num - 1))\
                                         || (string_len > 0 && (find_symbol->define_val > string_len - 1))){
                                            printf("********* error in file: %s line %d: %s\n    offset value %s defined by symbol is out of bounds\n", amFileName, line_counter, line, find_symbol->symName);
                                            is_error = TRUE;

                                        }    
                                    }
                                    else{
                                        printf("********* error in file: %s line %d: %s\n    symbol %s used but is undefined\n", amFileName, line_counter, line, INST_OP_DATA(ast,i).offset.label);
                                        is_error = TRUE;
                                    }
                                    
                                    add_to_code_image(translation_unit,find_symbol->address<<2);
                                    translation_unit->IC++;
                                }
                                else if(INST_OP_DATA(ast,i).offset.num != -1){
                                    /*if the value of the offset is bigger than the length of the array through which the offset is defined*/
                                    if(( data_arg_num > 0 && (INST_OP_DATA(ast,i).offset.num > (data_arg_num - 1))) \
                                    || (string_len > 0 && (INST_OP_DATA(ast,i).offset.num > (string_len - 1)))){
                                        printf("********* error in file: %s line %d: %s\n    offset value is out of bounds: %d\n", amFileName, line_counter, line, INST_OP_DATA(ast,i).offset.num);
                                        is_error = TRUE;
                                    }
                                    add_to_code_image(translation_unit,INST_OP_DATA(ast,i).offset.num<<2);
                                    translation_unit->IC++;
                                }
                            }

                        }
                        /*incase that symbol was used but undefined*/
                        else{
                            is_error = TRUE;
                            printf("********* error in file: %s line %d: %s\n    symbol is undefined: %s\n", amFileName, line_counter, line, ast->label_of_line);
                        }
                    }
                    /*incase operand is of const num type*/
                    else if(ast->operands.inst_ops[i].address_of_op == const_num){
                        if(INST_OP_DATA(ast,i).type_data == int_data)
                            add_to_code_image(translation_unit,INST_OP_DATA(ast,i).data_option.num << 2);
                        else if(INST_OP_DATA(ast,i).type_data == label_data){
                            find_symbol = does_symbol_exist(translation_unit->symbols, INST_OP_DATA(ast,i).data_option.label);
                            add_to_code_image(translation_unit,find_symbol->address << 2);
                        }
                        translation_unit->IC++;
                    }
                    
                } /*end for loop over the 2 operands of instruction*/
                
            } /*end else (ast->operands.inst_ops[0,1].reg_num == -1)*/

        } /*end if ast->typeofLine == inst*/
        line_counter++;
        free_ast(ast); /*free ast*/
        free(line); /*free line*/

    }

    free(extern1);
    return is_error; 
}

