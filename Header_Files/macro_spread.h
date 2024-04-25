#ifndef MACRO_SPREAD_H
#define MACRO_SPREAD_H

#include "global_var.h"

/**
 * @brief a structure representing a node of the list of deferent macros
 *        the node itself is a list that each node contains a line from the macro defintion
 * 
 */
struct macro_information {
    char *line;  
    struct macro_information* next;
};

/**
 * @brief a structure representing list of macros each macro is a list by isteslf where each node
 *  contains one line in the macro definition   
 * 
 */
struct macro {
    char *name;
    struct macro_information* info_head;   /* pointer to Head of the macro information linked list */
    struct macro* next;   /* Pointer to the next macro */
};

/*
 *  macro_definition: the line contains a macro definition.
 *  end_macro_definition: the line contains an end of macro definition.
 *  in_macro_definition: the line is within a macro definition.
 *  macro_call: the line contains a macro call.
 *  regular_line: the line does not fall into any of the above categories.
 */
enum {
    macro_definition,
    in_macro_definition,
    end_macro_definition,
    macro_call,
    regular_line
};
 
/**
 * @brief adds a new node to the list of a specific macro
 * 
 * @param m pointer to the head of the macro list 
 * @param line the line we want to add to the macro list
 */
void addMacroInformation(struct macro* m, const char* line);

/**
 * @brief adds a new macro to the macro list
 * 
 * @param macros pointer to the start of the macro list
 * @param name the name of the new macro
 */
void addMacro(struct macro** macros, const char* name, int * is_error);

void displayMacro_information(const struct macro* macro);
void displayMacros(const struct macro* macros);

/**
 * @brief frees the memory of a list of a specific macro
 * 
 * @param info a pointer a list of a specific macro list 
 */
void freeMacro_information(struct macro_information* info);

/**
 * @brief function to free all the memory used in the macros list,
 *  uses the free_macro_information to free each macro list.
 * 
 * @param macros a pointer to the list macros
 */
void free_Macros(struct macro* macros);


/**
 * @brief returns a pointer to the last macro in the macro list
 * 
 * @param macros the macro list
 * @return struct macro* pointer of the last macro
 */
struct macro *pointer_to_last_macro(struct macro** macros);

/**
 * @brief function to check if a certain string exists in the macro list
 * 
 * @param string a pointer to the string we want to check
 * @param original_macros a pointer to a pointer to the list of macros we want to search
 * @return struct macro* a pointer to the found marco, else returns null
 */
struct macro *is_string_macro(char *string, struct macro** original_macros);

/**
 * @brief function that can distinguish between a macro line and any other line,
 * if line is of type macro defintion it creates a macro for it in the macro list 
 * 
 * @param line a pointer to a string that is the line of text we want to classify
 * @param macros a pointer to our list of macros
 * @param macro a pointer for a specific macro list
 * @param is_error 
 * @return int representing the classification of the line
 *         macro_definition: the line contains a macro definition signal.
 *         end_macro_definition: the line contains an end of macro definition signal.
 *         in_macro_definition: the line is within a macro definition.
 *         macro_call: the line contains a call for a macro.
 *         regular_line: the line does not fall into any of the above categories.
 */
int macro_line_classifier(char * line, struct macro** macros, struct macro** macro, int * is_error);
/**
 * @brief the main function for the preprocessor process which finds and spreads macros in the file
 * 
 * @param name s pointer to string representing the name of the file we want to process
 * @return char* a pointer to a string holding the file name with the ".am" end
 */
char *macro_spread(char *name);

#endif