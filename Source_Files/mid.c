/*directive can have STRING[2] or STRING[len] but not STRING[LOOP[2]]*/

 #include "../Header_Files/vector.h"
#include "../Header_Files/mid.h"
#include "../Header_Files/global_var.h"
#include "../Header_Files/t_unit.h"
#include "../Header_Files/back.h"
#include "../Header_Files/ast.h"

/* part of adding new symbol struct to vector*/
void * symbol_ctor (const void * copy) {
    struct symbol * new_p = (struct symbol *) malloc(sizeof(struct symbol));
    if(new_p == NULL) 
        return NULL;  
    return memcpy(new_p,copy,sizeof(struct symbol));
}
/*delete function for vector  */
void symbol_dtor(void * item){
    free(item);
}

void * extern_ctor (const void * copy) {
    struct ext * new_p = (struct ext *) malloc(sizeof(struct ext));
    if(new_p == NULL) 
        return NULL; 
    return memcpy(new_p,copy,sizeof(struct ext));
}
/*delete function for vector  */
void extern_dtor(void * item){
    free(item);
}



struct symbol * does_symbol_exist(struct vector * symbol_vector, char * name){
    struct symbol * symbol_pointer;
    void *const *begin;
    void *const *end;
    VECTOR_FOR_EACH(begin, end, symbol_vector) {
        if(*begin){
            symbol_pointer = (struct symbol *) *begin;
            printf("comparing: %s to %s in symbol_vector\n", name, symbol_pointer->symName);
            if(strcmp(symbol_pointer->symName, name) == 0){
                printf("********* Symbol found in does_symbol_exist: %s\n", name);
                return symbol_pointer;
            }
        } 
    }
    printf("********* Symbol not found in does_symbol_exist: %s\n", name);
    return NULL;
}

struct ext * does_extern_exist(struct vector * extern_vector, char * name){
    struct ext * extern_pointer;
    void *const *begin;
    void *const *end;
    VECTOR_FOR_EACH(begin, end, extern_vector) {
        if(*begin) {
            extern_pointer = (struct ext *) *begin;
            if(strcmp(extern_pointer->ext_name, name) == 0){
                printf("********* extern found in does_extern_exist: %s\n", name);
                return extern_pointer;
            }
        } 
    }
    printf("********* extern not found in does_extern_exist: %s\n", name);
    return NULL;
}

void add_array_to_data_image(struct translation_unit *unit, int *array, size_t array_size) {
    int i;
    /* check if the translation_unit and the data_image are valid */
    if (unit == NULL || unit->data_image == NULL) {
        printf("Error: Invalid translation unit or data image, in func add_array_to_data_image\n");
        return;
    }
    /* iterate over the array and add each element to the data_image */
    for (i = 0; i < array_size; i++) {
        add_to_data_image(unit, array[i]);
        
    }
}



/***********************************************************************************************************************/
int firstPass(struct translation_unit *translation_unit, char *amFileName, FILE *amFile) {
    int i,len;
    char line[81];
    int ic = 100;
    int dc = 0;
    int is_error = FALSE;
    int line_counter = 1;

    frontend_ast ast = {0};
    struct symbol *find_symbol;
    void *const *begin;
    void *const *end;
    char *string1;

    /* Vector symbols_vector = new_vector(symbol_ctor, symbol_dtor); */
    struct symbol *symbol;
       

    printf("entering while loop in first pass\n");
    while (fgets(line, sizeof(line), amFile)) {
        symbol = (struct symbol *) malloc(sizeof(struct symbol)); 
        symbol->address =0;
        strcpy(symbol->symName,"\0");

        /* ast = front_end_func_simulator_multi_text(line_counter); */
        ast = frontend(line);

        if (ast.errors != NULL) {
            printf("********* syntax error in file: %s line: %d, %s\n", amFileName, line_counter, ast.errors);
            line_counter++;
            is_error = TRUE;
            continue;
        }
       
        printf("\n********* processing line number: %d\n",line_counter);
        printf("********* DC is: %d\nIC is: %d\n\n", dc, ic);

        /*check if their is a label in the line and if label is of code line or data\string type*/
        if (strlen(ast.label) > 0 && (ast.typeofLine == inst ||
            (ast.typeofLine == dir && (ast.operation_code.dir_code == dir_data || ast.operation_code.dir_code == dir_string)))) {
            printf("********* processing label: %s\n", ast.label);

            find_symbol = does_symbol_exist(translation_unit->symbols, ast.label);
            if (find_symbol) {
                if (find_symbol->symType == entrySymbol) {
                    if (ast.typeofLine == inst) {
                        symbol->symType = entryCodeSymbol;
                        symbol->address = ic;
                    } else {
                        symbol->symType = entryDataSymbol;
                        symbol->address = dc;
                    }
                } else {
                    printf("********* symbol found but not of .entry type\n");
                    printf("********* Semantic error in file: %s line: %d, redefinition of symbol\n", amFileName, line_counter);
                    is_error = TRUE;
                }
            }else{
                if (symbol == NULL) {
                    printf("********* Memory allocation error\n");
                    exit(1);
                }
                strcpy(symbol->symName, ast.label);
                if (ast.typeofLine == inst) {
                    symbol->symType = codeSymbol;
                    symbol->address = ic;
                } else {
                    symbol->symType = dataSymbol;
                    symbol->address = dc;
                }
                vector_insert(translation_unit->symbols, symbol);
            }
        }


            /* memcpy(translation_unit->data_image[translation_unit->DC],ast.operands.dir_ops.data_dir->data_option.num,); */
        if(ast.typeofLine == inst) {
                ic++;
                if(ast.operands.inst_ops[0].reg_num != -1 && ast.operands.inst_ops[1].reg_num != -1){
                    ic++;
                }
                else{
                    /*if one */
                    for(i = 0; i<2;i++){
                        if(ast.operands.inst_ops[i].reg_num != -1){
                            ic++;
                        }
                        else if(ast.operands.inst_ops[i].address_of_op == none){
                            /*do not add to ic*/
                        }
                        else if(ast.operands.inst_ops[i].reg_num == -1){
                            ic++;
                            if(ast.operands.inst_ops[i].address_of_op == label_offset){
                                ic++;
                            }
                        }
                    }
                }
        }else if(ast.typeofLine == dir) {
            if(ast.operands.dir_ops.data_dir->type_data == int_data || ast.operands.dir_ops.data_dir->type_data == label_data){
                dc += ast.operands.dir_ops.num_count;
            }
            else if(ast.operands.dir_ops.data_dir[0].type_data == string){
                dc += strlen(ast.operands.dir_ops.data_dir[0].data_option.string);
            }

        }
        if (ast.typeofLine == dir && ((ast.operation_code.dir_code == dir_entry) || (ast.operation_code.dir_code == dir_extern))) {
            find_symbol = does_symbol_exist(translation_unit->symbols, ast.operands.dir_ops.data_dir->data_option.label);
            if ((find_symbol != NULL) && (ast.operation_code.dir_code == dir_entry)) {
                if (find_symbol->symType == dataSymbol) {
                    find_symbol->symType = entryDataSymbol;
                } else if (find_symbol->symType == codeSymbol) {
                    find_symbol->symType = entryCodeSymbol;
                } else {
                    printf("********* label defined twice as entry or label defined but also defined as extern\n");
                    printf("********* Semantic error in file: %s line: %d, redefinition of symbol: %s\n", amFileName, line_counter, find_symbol->symName);
                    is_error = TRUE;
                }
            } else if (!find_symbol) {
                /* symbol = malloc(sizeof(struct symbol)); */
                if (symbol == NULL) {
                    printf("********* Memory allocation error\n");
                    exit(1);
                }
                strcpy(symbol->symName, ast.operands.dir_ops.data_dir->data_option.label);
                symbol->symType = ast.operation_code.dir_code;
                vector_insert(translation_unit->symbols, symbol);
            } else {
                printf("********* label wasn't already defined\n");
                printf("********* Semantic error in file: %s line: %d, redefinition of symbol: %s\n", amFileName, line_counter, find_symbol->symName);
                is_error = TRUE;
            }
        }else if(ast.typeofLine == define){
            find_symbol = does_symbol_exist(translation_unit->symbols,  ast.operands.dir_ops.data_dir[0].data_option.label);
            if(find_symbol){
                printf("error, symbol already defined\n");
            }
            else{
                strcpy(symbol->symName, ast.operands.dir_ops.data_dir[0].data_option.label);
                symbol->symType = ast.operation_code.dir_code;
                symbol->address = ast.operands.dir_ops.data_dir[1].data_option.num;
                vector_insert(translation_unit->symbols, symbol);

            }

        }
        /* free(symbol); does problems when freeing symbol, like not printing 00000000000001 1 after 00001010000100 644*/
        line_counter++;     
    }


    printf("\n********* checking to see if there is a symbol that was declared but not defined:\n");
    VECTOR_FOR_EACH(begin, end, translation_unit->symbols) {
        if (*begin) {
            symbol = (struct symbol *) *begin;
            printf("********* symbol: %s, type: %d, address: %d\n", symbol->symName, symbol->symType, symbol->address);

            if (symbol->symType == entrySymbol) {
                printf("********* symbol was declared but not defined: %s\n", symbol->symName);
                is_error = TRUE;
            }
            if (symbol->symType == dataSymbol || symbol->symType == entryDataSymbol) {
                symbol->address += ic;
            }
        }   
    }
    printf("\n");

    /*data_image part*/
    rewind(amFile);
    line_counter = 1;
    while (fgets(line, sizeof(line), amFile)) {     
        symbol = (struct symbol *) malloc(sizeof(struct symbol)); 
        symbol->address =0;
        strcpy(symbol->symName,"\0");
        /* ast = front_end_func_simulator_multi_text(line_counter); */
        ast = frontend(line);
        
            /*incase of declaration data/string, update the DC accordingly */
        printf("//////on line: %d in second while in first pass\n", line_counter);    
        if (ast.typeofLine == dir && (ast.operation_code.dir_code == dir_data || ast.operation_code.dir_code == dir_string)) {
            printf("/////////////////////////////in data_image adding, and updating DC\n");
            for(i=0; i<ast.operands.dir_ops.num_count; i++){
                if(ast.operands.dir_ops.data_dir[i].type_data == int_data){
                    printf("adding num: %d to data image\n", ast.operands.dir_ops.data_dir[i].data_option.num);
                    add_to_data_image(translation_unit,ast.operands.dir_ops.data_dir[i].data_option.num);
                    dc++;
                    translation_unit->DC++;
                }
                else if(ast.operands.dir_ops.data_dir[i].type_data == label_data){
                    printf("in label_data");
                    find_symbol = does_symbol_exist(translation_unit->symbols, ast.operands.dir_ops.data_dir[i].data_option.label);
                    if(find_symbol){
                        printf("adding label addres: %d to data image\n", find_symbol->address);
                        add_to_data_image(translation_unit, find_symbol->address);
                        dc++;
                        translation_unit->DC++;
                    }
                }
                else if(ast.operands.dir_ops.data_dir[i].type_data == string){
                    string1 = ast.operands.dir_ops.data_dir[i].data_option.string;
                    len = strlen(ast.operands.dir_ops.data_dir[i].data_option.string);
                    for (i = 0; i < len; i++) {
                        add_to_data_image(translation_unit, (int)string1[i]);
                        dc++;
                        translation_unit->DC++;
                    }
                }
                dc++;
            } 
        }
        line_counter++;
    }



    /* free(symbol); */
    return is_error;
}




int secondPass(struct translation_unit *translation_unit, char *amFileName, FILE *amFile){
    int i, is_op_source;
    char line[81];
    int line_counter = 1;
    int is_error = FALSE;
 /*    int ic = 100;
    int dc = 0;
    int is_error = FALSE;
     */

    
    frontend_ast ast = {0};
    struct ext *find_extern;
    struct symbol *find_symbol;
   /* void *const *begin;  
    void *const *end;
    struct ext *find_ext;  */

/*     Vector externals_vector = new_vector(extern_ctor, extern_dtor);
    struct ext *extern1; */
    struct ext *extern1 = (struct ext *) malloc(sizeof(struct ext));

    printf("********* Entering second pass\n");

    rewind(amFile);
    while (fgets(line, sizeof(line), amFile)) {
        is_op_source =0;

        printf("********* Processing line in second pass: %s\n", line);
        /* ast = front_end_func_simulator_multi_text(line_counter); */
        ast = frontend(line);
        printf("********* ast returned line in second pass: %s\n", line);
        
        /*incase that line is of type instruction */
        if(ast.typeofLine == inst){
            /*first binary word, same format for all code line*/
            /*source operand, bits 4-5 */
            if(ast.operands.inst_ops[0].address_of_op != none){
                printf("----------------------line 557, adding to code image: %d\n",ast.operands.inst_ops[0].address_of_op << 4);
                add_to_code_image(translation_unit,ast.operands.inst_ops[0].address_of_op << 4);
            }
            else{
                is_op_source++;
            }
            /*destination operand, bits 2-3 */
            if(ast.operands.inst_ops[1].address_of_op != none && is_op_source == 0){
                translation_unit->code_image[translation_unit->IC] |= ast.operands.inst_ops[1].address_of_op << 2;
            }
            else{
                printf("----------------------line 561, adding to code image: %d\n",ast.operands.inst_ops[1].address_of_op << 2);
                 add_to_code_image(translation_unit,ast.operands.inst_ops[1].address_of_op << 2);
            }
            /*opcode, bits 6-9*/
            translation_unit->code_image[translation_unit->IC] |= ast.operation_code.inst_code << 6;
            translation_unit->IC++;

            /* printf("reg num 0: %d \n reg num 1: %d\n", ast.operands.inst_ops[0].reg_num , ast.operands.inst_ops[1].reg_num ); */
            /*check if of two register type*/
            if(ast.operands.inst_ops[0].reg_num != -1 && ast.operands.inst_ops[1].reg_num != -1){
                printf("----------------------line 564, adding to code image: %d\n",ast.operands.inst_ops[0].reg_num << 5);
                add_to_code_image(translation_unit,ast.operands.inst_ops[0].reg_num << 5);
                translation_unit->code_image[translation_unit->IC] |= ast.operands.inst_ops[1].reg_num << 2;
                translation_unit->IC++;
            }
            else{
             
                for(i=0; i<2; i++){
                    /*if first operand is reg*/
                    if(ast.operands.inst_ops[0].address_of_op == reg && i == 0){
                        printf("----------------------line 574, adding to code image: %d\n",ast.operands.inst_ops[0].reg_num << 5);
                        add_to_code_image(translation_unit,ast.operands.inst_ops[0].reg_num << 5);
                        translation_unit->IC++;
                    }
                    /*if second operand is reg*/
                    if(ast.operands.inst_ops[1].address_of_op == reg && i == 1){
                        printf("----------------------line 580, adding to code image: %d\n",ast.operands.inst_ops[1].reg_num << 2);
                        add_to_code_image(translation_unit,ast.operands.inst_ops[1].reg_num << 2);
                        translation_unit->IC++;
                    }  
                    if(ast.operands.inst_ops[i].address_of_op == label || ast.operands.inst_ops[i].address_of_op == label_offset ){
                        printf("i is: %d\n", i);
                        printf("sending label: %s to does_symbol exist\n", ast.operands.inst_ops[i].data_inst.data_option.label);
                        find_symbol = does_symbol_exist(translation_unit->symbols, ast.operands.inst_ops[i].data_inst.data_option.label);
                        if(find_symbol){
                            printf("----------------------line 589, adding to code image: %d\n",find_symbol->address << 2);
                            add_to_code_image(translation_unit,find_symbol->address << 2);
                            printf("adding to code address of symbol: %d\n", find_symbol->address << 2);
                            /* printf("this is the main second key: %d\n", translation_unit->code_image[translation_unit->IC]); */
                            
                            if(find_symbol->symType == externSymbol){
                                /*extern case, ARE = 01*/
                                translation_unit->code_image[translation_unit->IC] |= 1;
                                find_extern = does_extern_exist(translation_unit->externals,find_symbol->symName); 

                                /*if extern already exists in externals vec */
                                if(find_extern){
                                    find_extern->address[find_extern->address_count] = translation_unit->IC + 100;
                                    find_extern->address_count++;
                                }
                                /*if extern doesn't exist in externals vec, we will add it */
                                else{
                                    extern1->address[0] = translation_unit->IC + 100;
                                    extern1->address_count++;
                                    extern1->ext_name = ast.operands.inst_ops[i].data_inst.data_option.label;
                                    vector_insert(translation_unit->externals, extern1); 
                                }                               
                            }
                            /*if symbol is not extern*/
                            else{
                                printf("in here, making ARE = 10");
                                translation_unit->code_image[translation_unit->IC] |= 2;
                            }
                            translation_unit->IC++;

                            /*incase that there is an offset*/
                            if(ast.operands.inst_ops[i].address_of_op == label_offset){
                                printf("in here aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
                                if(ast.operands.inst_ops[i].data_inst.offset.label != NULL){
                                    find_symbol = does_symbol_exist(translation_unit->symbols, ast.operands.inst_ops[i].data_inst.offset.label);
                                    printf("in here bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\n");
                                    printf("adding to code address: %d\n",find_symbol->address<<2);
                                    add_to_code_image(translation_unit,find_symbol->address<<2);
                                    translation_unit->IC++;
                                }
                                else if(ast.operands.inst_ops[i].data_inst.offset.num != -1){
                                    printf("in here bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\n");
                                    printf("adding to code address: %d\n",ast.operands.inst_ops[i].data_inst.offset.num);
                                    add_to_code_image(translation_unit,ast.operands.inst_ops[i].data_inst.offset.num);
                                    translation_unit->IC++;
                                }
                            }

                        }
                        /*incase that symbol was used but undefined*/
                        else{
                            is_error = TRUE;
                            printf("********* error in file: %s line: %d, undefined symbol: %s\n", amFileName, line_counter, ast.label);
                        }
                    }
                    /*incase operand is of const num type*/
                    else if(ast.operands.inst_ops[i].address_of_op == const_num){
                        printf("in inst_ops[i] = const_num (mion myady)\n");
                        if(ast.operands.inst_ops[i].data_inst.type_data == int_data)
                            add_to_code_image(translation_unit,ast.operands.inst_ops[i].data_inst.data_option.num << 2);
                        else if(ast.operands.inst_ops[i].data_inst.type_data == label_data){
                            find_symbol = does_symbol_exist(translation_unit->symbols, ast.operands.inst_ops[i].data_inst.data_option.label);
                            add_to_code_image(translation_unit,find_symbol->address << 2);
                        }
                        translation_unit->IC++;
                    }
                    
                }
            }

        }
        line_counter++;
        /*need to delete before sending.*/
        /*from here*/
        add_to_code_image(translation_unit,0);
        translation_unit->IC++;
        /*to here*/

    }

    printf("********* Exiting second pass\n");
    free(extern1);
    return is_error; 
}

