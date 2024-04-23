#include <stdio.h>
#include <ctype.h>
#include <errno.h> 
#include <string.h>
#include <stdlib.h> 
#include "../Header_Files/pre.h"
#include "../Header_Files/global_var.h"
#include "../Header_Files/ast.h"



/*checking to see if the name is a system reserved word
which includes */
int is_system_name(char name[MAX_LINE_LEN]) {
    
    /* checking if the recieved name is either:
    a register - one from r0,r1,....,r7
    the instructions - mov,add, etc....
    the directives - data,string,extern,entry,define*/
    if (is_reg(name) != -1 || is_dir(name) != -1 || is_inst(name) != -1) {
        return TRUE;
    }
    /*if the name is either the macro definition names - mcr,endmcr*/
    if(strcmp(name, "mcr") == 0 || strcmp(name, "endmcr") == 0) return TRUE;
    
    return FALSE;
}

/*  Function to add a new line of information to a macro */
void addMacroInformation(struct macro* m, const char* line) {
    
    int line_len;
    struct macro_information* current = NULL;
    struct macro_information* info = (struct macro_information*)malloc(sizeof(struct macro_information));
    if (info == NULL) {
        /*  Handle memory allocation error */
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    line_len = strlen(line);

    info->line = (char*)malloc((line_len+1)*sizeof(char));
    if(info->line == NULL){
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    
    strncpy(info->line, line, line_len);
    info->line[line_len] = '\0';   /* null-termination */
    info->next = NULL;

     /* If the macro information list is empty, set the new info as the head */
    if (m->info_head == NULL) {
        m->info_head = info;
    } else {
        /*  Otherwise, move on the list and append the new info */
        current = m->info_head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = info;
    }
}

/*  Function to add a new macro to the macro list */
void addMacro(struct macro** macros, const char* name, int * is_error) {
    struct macro* m = (struct macro*)malloc(sizeof(struct macro));
    size_t len;
    if (m == NULL) {
         /* Handle memory allocation error */
        fprintf(stderr, "Memory allocation error\n");
    }

    len = strlen(name);
    if (len > 0 && name[len - 1] == '\n') {
        /* Remove the newline character if present at the end */
        strncpy(m->name, name, len - 1);
        m->name[len - 1] = '\0';
    } else {
        strncpy(m->name, name, MAX_LINE_LEN - 1);
        m->name[MAX_LINE_LEN - 1] = '\0';
    }
    m->info_head = NULL;
    m->next = NULL;

     /* If the macro list is empty, set the new macro as the head */
    if (*macros == NULL) {
        *macros = m;
        printf("checking if name is protected\n");
        if(is_system_name(m->name)){
            printf("error: name used for macro is a system reserved name\n");
        }
    } else {
        
         /* Otherwise, move on the list and append the new macro */
        struct macro* current = *macros;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = m;
        printf("checking if name is protected\n");
        if(is_system_name(m->name)){
            printf("error: name used for macro is a system reserved name\n");
            *is_error = TRUE;
        }
    }
}


void displayMacro_information(const struct macro* macro) {
    struct macro_information* info = macro->info_head;
        while (info != NULL) {
            printf("  %s\n", info->line);
            info = info->next; 

        }
}

/* Function to display the content of the macros */
void displayMacros(const struct macro* macros) {
    const struct macro* current = macros;
    while (current != NULL) {
        printf("Macro Name: %s\n", current->name);
        displayMacro_information(current);
        current = current->next;
    }
}


 void freeMacro_information(struct macro_information* info) {

    struct macro_information* temp;

    if(!info){
        printf("info is NULL in free_macros\n");
    }
    /*iterates throw the list to free all the macro information*/
        while (info != NULL) {
            temp = info->next;
            free(info->line);
            free(info);
            info = temp;
        }        
}

void free_Macros(struct macro* macros) {
    struct macro* next_macro = macros->next;
    if(!macros){
        printf("macros is NULL in free_macros\n");
    }
    /*irritates throw the list to free all the macros*/
    while (macros != NULL) {
        next_macro = macros->next;
        freeMacro_information(macros->info_head);
        free(macros);
        macros = next_macro;
    }
}


struct macro * pointer_to_last_macro(struct macro** macros){
    struct macro* current = *macros;
    struct macro* nextMacro = current->next;
    printf("in pointer_to_last_macro func\n");
    while(nextMacro){
        current = nextMacro;  
        nextMacro = nextMacro->next;
    }
    if(*macros){
        printf("macros is NOT null in pointer_to_last_macro*********************\n");
    }
    return current;
} 

 
struct macro * is_string_macro(char * string, struct macro** original_macros){
    int i;
    struct macro* macros = *original_macros;
    printf("in is_string_macro func/////\n");
    printf("string: ");
    for (i = 0; string[i] != '\0'; ++i) {
        printf("%d ", string[i]);
    }
    printf("\n");

    if(!macros){
        printf("*macros is Null\n");
    }

    while(macros){
        printf("macros->name: ");
        for (i = 0; macros->name[i] != '\0'; ++i) {
            printf("%d ", macros->name[i]);
        }
        printf("\n");

        if(strcmp(macros->name, string) == 0){
            printf("is_string_macro returns macros\n");
            return macros;
        }
        macros = macros->next;
    }

    printf("is_string_macro returns null\n");
    return NULL;
}

int macro_line_classifier(char * line, struct macro** macros, struct macro** macro, int * is_error){
    struct macro* wanted_macro = NULL;
    char *line_copy;
    char *token;
    
    if(!(*macros)){
        printf("macros is null in macro_line_classifier*********\n");
    }

    printf("Processing line: %s\n", line);

    /* if it's an empty line - just treat it as a regular line*/
    

    line_copy = (char *)malloc(strlen(line) + 1);
    if(!line_copy){
        printf("********* Memory allocation error\n");
        return -1;
    }
    strcpy(line_copy, line);
    
    if(isEmptyString(line_copy)){
        free(line_copy);
        return regular_line;
    }
    
    token = strtok(line_copy, SPACES);
    token = trimStartEnd(token);/*first token of line without spaces */
    printf("token: %s\n", token);
    if(strcmp(token, "mcr") == 0){
        token = strtok(NULL, " "); /*name of macro*/
        printf("in macro_definition, token: %s\n", token);
        if (token) {
            addMacro(macros, token, is_error); /*create macro struct with the name of the macro */
            printf("adding first macro *******************************************************************\n");
            if(!macros){
                printf("macros is null*********************\n");
            }
            else{
                printf("macros is NOT null*********************\n");
            }
            *macro = pointer_to_last_macro(macros);
            free(line_copy);
            return macro_definition;
        }

    }
    else if(strcmp(token, "endmcr") == 0){
        printf("returning end_macro_definition-----------------------------------\n");
        free(line_copy);
        return end_macro_definition;
    }
    else if(*macro){
        printf("adding %s to %s\n",line, (*macro)->name);
        addMacroInformation(*macro, line);
        free(line_copy);
        return in_macro_definition;
    }
    else if((wanted_macro = is_string_macro(token, macros))){
        printf("in macro_call...............");
        printf("wanted macro name:%s  \n", (wanted_macro)->name);
        *macro = wanted_macro;
        free(line_copy);
        return macro_call;
    }
    else{
        free(line_copy);
        return regular_line;
    }

    free(line_copy);
    return 0;
}
 /*This function reads a from an input file a line terminated by newline character
 with variable length. if the first character read c is a newline character
 then just the newline is returned*/
char* read_line_input(char c,FILE* file){
    char* inputLine = NULL;
    char str_tmp[CHUNK_READ];
    int len_input = 0, len_tmp = 0;

    /*rellocating to get the character c in there*/
    inputLine = (char *) calloc(2, sizeof(char));
    if(!inputLine){
        printf("********* Memory allocation error\n");
    }
    strncpy(inputLine,&c,1); /*putting the character into the inputline*/
    len_input++;

    /*if the character was a newline character, then we just return the newline*/
    if(c == '\n') return inputLine; 
    do {
        fgets(str_tmp, CHUNK_READ, file); /*reading characters in chuncks from file*/
        len_tmp = strlen(str_tmp); /*reading the real length of the string recieved*/
        inputLine = (char *) realloc(inputLine, len_input+len_tmp+1); /*reallocating according to what we recieved*/
        if(!inputLine){
            printf("********* Memory allocation error\n");
        }
        strcpy(inputLine+len_input, str_tmp);
        len_input += len_tmp;
    } while (len_tmp==CHUNK_READ-1 && str_tmp[CHUNK_READ-2]!='\n');
    
    return inputLine;
}



char * preprocessor(char * name){
    int is_error = FALSE;

    struct macro* macros = NULL; /*data structure to holed the macros*/
    struct macro* macro = NULL;
    struct macro_information* current_info = NULL;
    char *line,c;         
    FILE * file_as; /*the original file*/
    FILE * file_am; /*the file containing the assembly code after the preprocessor */
    char *as_extension = ".as";
    char *am_extension = ".am";

    /*allocate memory and concentrate the strings to get their full name*/
    char *file_as_name = (char *)malloc(strlen(name) + strlen(as_extension) + 1); 
    char *file_am_name = (char *)malloc(strlen(name) + strlen(am_extension) + 1); 
    strcpy(file_as_name, name);
    strcpy(file_am_name, name);
    strcat(file_as_name, as_extension);
    strcat(file_am_name, am_extension);

    file_as = fopen(file_as_name, "r"); /*read file*/
    file_am = fopen(file_am_name, "w"); /*create file*/ 

    /*check that something didn't go wrong in the fopen func*/
    if(!file_as || !file_am){
        fprintf(stderr, "Error opening files\n");
        return NULL;
    }

    while((c = fgetc(file_as)) != EOF){
        line = read_line_input(c, file_as);
        
        switch(macro_line_classifier(line, &macros, &macro, &is_error)){
            case 0: /*macro_definition*/
                break;
            case 1: /* in_macro_definition */
                break;
            case 2: /* end_macro_definition */
                printf("in case 2(end_macro)\n");
                macro = NULL;
                current_info = NULL;
                break;
            case 3: /*macro_call*/  
                current_info = macro->info_head;
                while(current_info){
                    fputs(current_info->line,file_am);
                    current_info = current_info->next;
                }
                macro = NULL;
                current_info = NULL;
                break;
            case 4: 
                printf("adding line: %s to file_am\n", line);
                fputs(line, file_am);   
                break;   
        }
        free(line);
    }
    
    if(macros != NULL){
        printf("call for displayMacros:\n");
        displayMacros(macros);
        free_Macros(macros);
    }
    free(file_as_name);
/*     free(file_am_name); */
/*     macros = NULL; */
    fclose(file_as);
    fclose(file_am);
    if(is_error){
        free(file_am_name);
        return NULL;       
    }
    return file_am_name;
    
    /*NEED TO  FREE FILE AM NAME */

}
