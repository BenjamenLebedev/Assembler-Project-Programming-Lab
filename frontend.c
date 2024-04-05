#include "ast_lib.h"
#include "global_var.h"

frontend_ast frontend(char* line){

    frontend_ast *ast; 
    char *copy_line,*char_skip,*label,**saveptr;
    char *start,*end_newline;
    int check_dir,check_inst;

    /* Allocate memory for the AST */
    ast = (frontend_ast *) calloc(1, sizeof(frontend_ast));
    if(ast == NULL){
        /*fprintf(stderr, "Memory allocation failed\n");*/
        return (*ast);
    }

    /* initialize the AST */
    frontend_init(ast);

    /* Allocate memory for the saveptr to use in tokenization in other processes*/
    saveptr = (char**) calloc(1,sizeof(char*));
    if(!saveptr){
        strcpy((*ast).errors, "Memory allocation failed\n");
        return (*ast);
    }

    /*If length of the line exceeds the maximal length - we skip that line*/
    if(strlen(line) > MAX_LINE_LEN){
        strcpy((*ast).errors, "Assembly line too long - maximal length is 82 characters\n");
        (*ast).typeofLine = error;
        return (*ast);
    }

    /* removing the newline character from the end of the line*/ 
    if(line[strlen(line) - 1] == '\n'){
        copy_line = (char*) calloc(strlen(line) + 1, sizeof(char));
        if(copy_line == NULL){
            (*ast).typeofLine = error;
            return (*ast);
        }
        start = line;
        end_newline = strchr(line, '\n');
        memcpy(copy_line, start, end_newline - start);
        copy_line[end_newline - start] = '\0';
        line = copy_line;
    }
    /* a case where a newline character is somewhere in the middle*/
    if(strchr(line, '\n') && line[strlen(line) - 1] != '\n'){
        strcpy((*ast).errors, "a newline character appears in the middle of the line\n");
        (*ast).typeofLine = error;
        return (*ast);
    } 

    /* found ; signifying it's a possible comment line*/
    if(strchr(line, ';')){
        if(line[0] == ';'){
            (*ast).typeofLine = empty;
            return (*ast);
        }
        else{
            strcpy((*ast).errors, "the character ; appears illegally in the middle of the line\n");
            (*ast).typeofLine = error;
            return (*ast);
        }
    }

    /* trimming the line from extra spaces at the start and at the end - since those 
    do not contribute to anything, save for a migrane*/
    line = trimStartEnd(line);

    /* found : signifying there's a possible label at the start of the line*/
    if((char_skip = strchr(line, ':'))){
        if(!(label = check_legal_label(ast, line,0, saveptr))){ /*checking the label*/
            if((*ast).errors[0] == '\0') strcpy((*ast).errors, "Illegal declaration of label\n");
            (*ast).typeofLine = error;
            (*ast).label[0] = '\0';
            return (*ast);
        }
        else strcpy((*ast).label, label);
    }

    /* If a label was found, we move past it to analyze the rest of the line*/
    if(char_skip && label) line = char_skip + 1;
    line = trimStartEnd(line); /*again trimming start and end white space to isolate the operation*/

    /* found . signifying it's a possible directive line */
    if((char_skip = strchr(line, '.')) && find_dir(line)){
        check_dir = check_directive(ast, line, saveptr); /*checking legallity of directive*/
        if(!check_dir){
            (*ast).typeofLine = error;
            return (*ast);
        }
    }
    /* found no . meaning there's only one other possible option - either
    it's an instruction or another migrane */
    else{
        check_inst = check_instruction(ast, line, saveptr);
        if(!check_inst){
            (*ast).typeofLine = error;
            return (*ast);
        }
    }

    free(saveptr);
    return (*ast);
}