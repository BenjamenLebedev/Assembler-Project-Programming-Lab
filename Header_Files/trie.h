/* The library to implement a trie structure to search for strings and print their occurences*/
#ifndef TRIE_H
#define TRIE_H

#include "../Header_Files/global_var.h"
/*
#define TRIE_START_CHAR ' ' // the start character from which we count the children of each node
#define ALPHABET_SIZE 95    // the size of the alphabet in ascii from ' ' to '~'
*/
#define TRIE_START_CHAR '0' // the start character from which we count the children of each node
#define ALPHABET_SIZE 74    // the size of the alphabet in ascii from '0' to 'z'

typedef struct data_of_string {
    char *symbol_name; /* name of symbol - null if the node does not represent a symbol name*/
    symFlag sym_flag; /* the flag of the symbol - extern or not*/
    struct ext * ext_ptr; /* pointer to the external symbol */
    struct symbol * symbol_ptr; /* pointer to the symbol */
    int ext_count; /* the number of times that an external symbol is used */
    int word_count; /* the number of appearances of the word - with each line considered as one appearance */
    int *line_appears; /* the line numbers at which symbol_name appears */
} data_trie;

typedef struct trie_node {
    data_trie symbol_data;
    struct trie_node * children[ALPHABET_SIZE]; /* representing all possible ascii characters*/
} trie_node;

/* the trie is basically the root of the tree */
typedef struct trie {
    trie_node *children[ALPHABET_SIZE]; 
} trie_tree;

typedef trie_tree* Trie;

/**
 * @brief create and returns a new Trie data structure.
 * 
 * @return A trie data structure with allocated memory.
 */
Trie create_trie();

/**
 * @brief inserts a string into the Trie data structure, with recording of the line number it appeared in.
 * 
 * @param trie reference to the Trie structure.
 * @param str the string to insert in the trie.
 * @param line_num the line number of the string.
 * @return  pointer to the string if it exists, NULL otherwise.
 */
const char *trie_insert(Trie trie,const char *str,int line_num);

/**
 * @brief checks if a string exists in the Trie data structure.
 * 
 * @param trie reference to the Trie structure.
 * @param str the string to insert in the trie.
 * @return  pointer to the string if it exists, NULL otherwise.
 */
char* trie_check_exists(Trie trie,const char *str,int line_num);

/**
 * @brief updates the trie node with the string str and the line number.
 * 
 * @param node the node to update.
 * @param str the string to insert in the trie.
 * @param line_num the line number of the string.
 * @return 1 if the update was successful, 0 otherwise (mainly due to alloc errors).
 */
int trie_update_node(trie_node **node,const char *str,int line_num);

/**
 * @brief prints all the strings in the trie in lexicographical order.
 * 
 * @param trie reference to the Trie structure.
 * @param print_func the function to print the data of the string in the node.
 * @param ptr the data of the string.
 */
void trie_travel(Trie trie, void (*print_func)(data_trie *ptr),data_trie * ptr);

/**
 * @brief deletes a string from the Trie data structure.
 * 
 * @param trie reference to the Trie structure.
 * @param str the string to insert in the trie.
 * @param line_num the line number of the string.
 */
void trie_delete(Trie trie,const char *str,int line_num);

/**
 * @brief completely deallocates all memory for the Trie. destroying it completey.
 * 
 * @param trie the trie structure to be destroyed.
 */
void trie_full_delete(Trie * trie);

/* the printing function for the trie - the st */
void my_print_func(data_trie *ptr);

#endif 