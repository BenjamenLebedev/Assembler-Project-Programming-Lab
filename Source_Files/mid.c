/*directive can have STRING[2] or STRING[len] but not STRING[LOOP[2]]*/

#include "../Header_Files/vector.h"
#include "../Header_Files/mid.h" 
#include "../Header_Files/global_var.h"
#include "../Header_Files/t_unit.h"
#include "../Header_Files/back.h"
#include "../Header_Files/ast.h"

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



struct symbol * does_symbol_exist(struct vector * symbol_vector, char * name){
    struct symbol * symbol_pointer;
    void *const *begin;
    void *const *end;
    /*iterate through the symbols vec*/
    LOOP(begin, end, symbol_vector) {
        if(*begin){
            symbol_pointer = (struct symbol *) *begin;
            printf("comparing: %s to %s in symbol_vector\n", name, symbol_pointer->symName);
            /*if the symbol name matches the provided name*/
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
    /*iterate through the symbols vec*/
    LOOP(begin, end, extern_vector) {
        if(*begin) {
            extern_pointer = (struct ext *) *begin;
            /*if the symbol name matches the provided name*/
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
    char line[MAX_LINE_LEN];
    int ic = IC_START;
    int dc = 0;
    int is_error = FALSE;
    int line_counter = 1;

    frontend_ast ast = {0};
    struct symbol *find_symbol;
    void *const *begin;
    void *const *end;
    char *string1;

    struct symbol *symbol;
       

    printf("entering while loop in first pass\n");
    while (fgets(line, sizeof(line), amFile)) {
        symbol = (struct symbol *) malloc(sizeof(struct symbol)); 
        symbol->address =0;
        strcpy(symbol->symName,"\0");
        symbol->is_symbol_define = FALSE;

        printf("\n\n\n\n********* processing line number: %d\n",line_counter);
        printf("********* DC is: %d\nIC is: %d\n\n", dc, ic);

        ast = frontend(line);/*get the ast struct for each line*/

        /*check if ast found errors, if so print them and move to the next line*/
        if (ast.errors != NULL) {
            printf("********* error: syntax error in file: %s line: %d, %s\n", amFileName, line_counter, ast.errors);
            line_counter++;
            is_error = TRUE;
            continue;
        }/*check if line is empty, if so move on*/
        else if(ast.typeofLine == empty){
            /* line_counter++; */
            continue;
        }
       
        /*check if their is a label in the line and if label is of code line or data\string type*/
        if (strlen(ast.label) > 0 && (ast.typeofLine == inst ||
            (ast.typeofLine == dir && (ast.operation_code.dir_code == dir_data || ast.operation_code.dir_code == dir_string)))) {
            printf("********* processing label: %s\n", ast.label);
            /*check to see if label all ready exists*/
            find_symbol = does_symbol_exist(translation_unit->symbols, ast.label);
            if (find_symbol) { /*is symbol exists*/
                /*change type of symbol accordingly*/
                if (find_symbol->symType == entrySymbol) {
                    if (ast.typeofLine == inst) {
                        find_symbol->symType = entryCodeSymbol;
                        find_symbol->address = ic;
                    } else {
                        printf("changing to entryDataSymbol\n");
                        find_symbol->symType = entryDataSymbol;
                        find_symbol->address = dc;
                    }
                    translation_unit->entry_use = TRUE;
                } else {/*if symbol all ready exists and not of type entry then it has to be a redefinition error*/
                    printf("********* symbol found but not of .entry type\n");
                    printf("********* Semantic error in file: %s line: %d, redefinition of symbol\n", amFileName, line_counter);
                    is_error = TRUE;
                }
            }else{/*if symbol doesn't all ready exist, we will add it to symbol vector*/
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

        /*if line is of instruction type*/
        if(ast.typeofLine == inst) {
                /*we will update ic accordingly depending on the line*/
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
                /*else, if line is of type directive, we will update dc accordingly */
        }else if(ast.typeofLine == dir && (ast.operation_code.dir_code == dir_data || ast.operation_code.dir_code == dir_string )) {
            if(ast.operands.dir_ops.data_dir->type_data == int_data || ast.operands.dir_ops.data_dir->type_data == label_data){
                dc += ast.operands.dir_ops.num_count;
            }
            else if(ast.operands.dir_ops.data_dir[0].type_data == string){
                dc += strlen(ast.operands.dir_ops.data_dir[0].data_option.string) +1;
            }
        }

        /*incase line is of type extern or entry*/
        if (ast.typeofLine == dir && ((ast.operation_code.dir_code == dir_entry) || (ast.operation_code.dir_code == dir_extern))) {
            /*find if symbol all ready exists*/
            find_symbol = does_symbol_exist(translation_unit->symbols, ast.operands.dir_ops.data_dir->data_option.label);
            if ((find_symbol != NULL) && (ast.operation_code.dir_code == dir_entry)) {
                /*if symbol found update symbol type if symbol wasn't all ready defined as extern or entry*/
                if (find_symbol->symType == dataSymbol) {
                    printf("changing to entry data symbol");
                    find_symbol->symType = entryDataSymbol;
                    translation_unit -> entry_use = TRUE;
                } else if (find_symbol->symType == codeSymbol) {
                    printf("changing to entry code symbol");
                    find_symbol->symType = entryCodeSymbol;
                    translation_unit -> entry_use = TRUE;
                } else {
                    /*if symbol is defined as entry or extern but all ready was defined as 
                    entry or extern then it has to be a error*/
                    printf("********* label defined twice as entry or label defined but also defined as extern\n");
                    printf("********* Semantic error in file: %s line: %d, redefinition of symbol: %s\n", amFileName, line_counter, find_symbol->symName);
                    is_error = TRUE; /*change error flag to TRUE*/
                }
            } /*else, if symbol doesn't all ready exist, we will add it*/
            else if (!find_symbol) {       
                strcpy(symbol->symName, ast.operands.dir_ops.data_dir->data_option.label);
                symbol->symType = (symType) ast.operation_code.dir_code;
                vector_insert(translation_unit->symbols, symbol);
            } else {
                printf("********* label wasn't already defined\n");
                printf("********* Semantic error in file: %s line: %d, redefinition of symbol: %s\n", amFileName, line_counter, find_symbol->symName);
                is_error = TRUE;
            }
        /*if line is of type define */
        }else if(ast.typeofLine == define){
            /*check to see if symbol all ready exists*/
            find_symbol = does_symbol_exist(translation_unit->symbols,  ast.operands.dir_ops.data_dir[0].data_option.label);
            if(find_symbol){
                /*is symbol all ready exists then it has to be a redefinition error*/
                printf("error, symbol already defined\n");
            }
            /*else, wee will add symbol to symbols vector*/
            else{
                strcpy(symbol->symName, ast.operands.dir_ops.data_dir[0].data_option.label);
                /* symbol->symType = ast.operation_code.dir_code; */
                symbol->symType = dataSymbol;
                symbol->address = ast.operands.dir_ops.data_dir[1].data_option.num;
                symbol->is_symbol_define = TRUE;
                vector_insert(translation_unit->symbols, symbol);
            }

        }
        line_counter++;     /*increase line counter each while loop*/
        frontend_free(&ast); /*free ast*/
    }


    printf("\n********* checking to see if there is a symbol that was declared but not defined:\n");
    LOOP(begin, end, translation_unit->symbols) {
        if (*begin) {
            symbol = (struct symbol *) *begin;
            printf("********* symbol: %s, type: %d, address: %d\n", symbol->symName, symbol->symType, symbol->address);

            if (symbol->symType == entrySymbol) {
                printf("********* error: symbol was declared but not defined: %s\n", symbol->symName);
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
    while (fgets(line, sizeof(line), amFile)) {     
        symbol = (struct symbol *) malloc(sizeof(struct symbol)); 
        symbol->address =0;
        strcpy(symbol->symName,"\0");

        ast = frontend(line); /*get the ast*/
        
        /*incase of declaration data/string, update the DC accordingly */
        printf("//////on line: %d in second while in first pass\n", line_counter);        
        if (ast.typeofLine == dir && (ast.operation_code.dir_code == dir_data || ast.operation_code.dir_code == dir_string)) {
            printf("/////////////////////////////in data_image adding, and updating DC\n");
            /*iterate number of times by length of string or amount of integers */
            for(i=0; i<ast.operands.dir_ops.num_count; i++){
                /*if data is integers*/
                if(ast.operands.dir_ops.data_dir[i].type_data == int_data){
                    printf("adding num: %d to data image\n", ast.operands.dir_ops.data_dir[i].data_option.num);
                    add_to_data_image(translation_unit,ast.operands.dir_ops.data_dir[i].data_option.num);
                    dc++;
                    translation_unit->DC++;
                }
                /*if data is label data*/
                else if(ast.operands.dir_ops.data_dir[i].type_data == label_data){
                    printf("in label_data");
                    find_symbol = does_symbol_exist(translation_unit->symbols, ast.operands.dir_ops.data_dir[i].data_option.label);
                    if(find_symbol){
                        printf("adding label addres: %d to data image\n", find_symbol->address);
                        add_to_data_image(translation_unit, find_symbol->address);
                        dc++;
                        translation_unit->DC++;/*add zero for end of string*/
                    }
                }
                /*if data is of string type*/
                else if(ast.operands.dir_ops.data_dir[i].type_data == string){
                    printf("gggggggggggggggggggggggggggggggggggggggggggggg");
                    string1 = ast.operands.dir_ops.data_dir[i].data_option.string;
                    len = strlen(ast.operands.dir_ops.data_dir[i].data_option.string);
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
        frontend_free(&ast); /*free ast*/
    }



   /*  free(symbol);  *//*can cause probemlms*/
    return is_error;
}




int secondPass(struct translation_unit *translation_unit, char *amFileName, FILE *amFile){
    int i, is_op_source;
    char line[MAX_LINE_LEN];
    int line_counter = 1;
    int is_error = FALSE;

    frontend_ast ast = {0};
    struct ext *find_extern;
    struct symbol *find_symbol;

    struct ext *extern1 = (struct ext *) malloc(sizeof(struct ext));

    printf("********* Entering second pass\n");

    rewind(amFile);
    while (fgets(line, sizeof(line), amFile)) {
        
        
        printf("********* Processing line in second pass: %s\n", line);
        /* ast = front_end_func_simulator_multi_text(line_counter); */
        ast = frontend(line);
        printf("********* ast returned line in second pass: %s\n", line);
        
        /*empty line */
        if(ast.typeofLine == empty){
            continue;
        }
        is_op_source =0;

        /*incase that line is of type instruction */
        if(ast.typeofLine == inst){
            /*first binary word, same format for all code line*/
            /*source operand, bits 4-5 */
            if(ast.operands.inst_ops[0].address_of_op == none && ast.operands.inst_ops[1].address_of_op == none){
                printf("line 423, in two none case\n");
                add_to_code_image(translation_unit,0);
            }
            if(ast.operands.inst_ops[0].address_of_op != none){
                printf("----------------------line 557, adding to code image: %d\n",ast.operands.inst_ops[0].address_of_op << 4);
                add_to_code_image(translation_unit,ast.operands.inst_ops[0].address_of_op << 4);
            }
            else{
                is_op_source++;
            }
            /*destination operand, bits 2-3 */
            if(ast.operands.inst_ops[1].address_of_op != none && is_op_source == 0){
                printf("adding in line 434");
                translation_unit->code_image[translation_unit->IC] |= ast.operands.inst_ops[1].address_of_op << 2;
            }
            else if(ast.operation_code.inst_code != op_hlt && ast.operation_code.inst_code != op_rts){
                printf("----------------------line 449, adding to code image: %d\n",ast.operands.inst_ops[1].address_of_op << 2);
                add_to_code_image(translation_unit,ast.operands.inst_ops[1].address_of_op << 2);
            }
            /*opcode, bits 6-9*/
            printf("blalalalala ast.operation_code.inst_code << 6: %d\n",ast.operation_code.inst_code << 6);
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
                printf("this is it4:\n");
                printBinary14(translation_unit->code_image[translation_unit->IC]);
                printf("this is it4:\n");
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
                            if(find_symbol->symType != externSymbol){
                                printf("----------------------line 589, adding to code image: %d\n",find_symbol->address << 2);
                                add_to_code_image(translation_unit,find_symbol->address << 2);
                                printf("adding to code address of symbol: %d\n", find_symbol->address << 2);
                                /* printf("this is the main second key: %d\n", translation_unit->code_image[translation_unit->IC]); */
                            }

                            if(find_symbol->symType == externSymbol){
                                /*extern case, ARE = 01*/
                                printf("in find_symbol->symType == externSymbol\n");
                                add_to_code_image(translation_unit,0);
                                translation_unit->code_image[translation_unit->IC] |= 1;
                                /* translation_unit->IC++; */
                                find_extern = does_extern_exist(translation_unit->externals,find_symbol->symName); 

                                /*if extern already exists in externals vec */
                                if(find_extern){
                                    find_extern->address[find_extern->address_count] = translation_unit->IC + 100;
                                    find_extern->address_count++;
                                    translation_unit->extern_use = TRUE;
                                }
                                /*if extern doesn't exist in externals vec, we will add it */
                                else{
                                    printf("adding extern\n");
                                    extern1->address[0] = translation_unit->IC + 100;
                                    extern1->address_count = 1;
                                    translation_unit->extern_use = TRUE;
                                    extern1->ext_name = ast.operands.inst_ops[i].data_inst.data_option.label;
                                    vector_insert(translation_unit->externals, extern1); 
                                }                               
                            }/*if symbol is not extern*/
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
                                    printf("adding to code address: %d\n",ast.operands.inst_ops[i].data_inst.offset.num<<2);
                                    add_to_code_image(translation_unit,ast.operands.inst_ops[i].data_inst.offset.num<<2);
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
        frontend_free(&ast); /*free ast*/

    }

    printf("********* Exiting second pass\n");
    free(extern1);
    return is_error; 
}

