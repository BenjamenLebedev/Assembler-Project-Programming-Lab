#include "../Header_Files/ast.h"
#include "../Header_Files/global_var.h"

void print_ast(frontend_ast *ast);
   
int mainss(){

    frontend_ast *ast;
    char *line = "STR: rts  \n";

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
