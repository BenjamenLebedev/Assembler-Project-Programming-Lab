#ifndef TRIE_H
#define TRIE_H

/* The library to implement a trie structure to search for strings and print their occurences*/

#define TRIE_START_CHAR ' ' /* the start character from which we count the children of each node */
#define ALPHABET_SIZE 95 /* the size of the alphabet in ascii */

typedef struct data_of_string {
    char *word_str; /* a string that is null if the node does not represent a valid string*/
    int word_count; /* the number of appearances of the word - with each line considered as one appearance */
    int *line_appears; /* the line numbers at which word_str appears */
} data;

typedef struct trie_node {
    data string_data;
    struct trie_node * children[ALPHABET_SIZE]; /* representing all possible ascii characters*/
} trie_node;

/* the trie is basically the root of the tree */
typedef struct trie {
    trie_node * children[ALPHABET_SIZE]; 
} trie_tree;

typedef struct trie* Trie;

/**
 * @brief create and returns a new Trie data structure.
 * 
 * @return A trie data structure with allocated memory.
 */
Trie trie();

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
 */
void trie_update_node(trie_node **node,const char *str,int line_num);

/**
 * @brief prints all the strings in the trie in lexicographical order.
 * 
 * @param trie reference to the Trie structure.
 * @param print_func the function to print the data of the string in the node.
 * @param ptr the data of the string.
 */
void trie_travel(Trie trie, void (*print_func)(data *ptr),data * ptr);

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
 * @param trie 
 */
void trie_full_delete(Trie * trie);

/* the printing function for the trie - the st */
void my_print_func(data *ptr);

#endif