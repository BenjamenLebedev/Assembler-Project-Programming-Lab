#include "../Header_Files/global_var.h"
#include "../Header_Files/trie.h"

/* Functions concerning maintaining a Trie structure - that stores strings and records their appearances 
in each line of input.*/

/* destroy the trie node and all of its children */
static void trie_destroy_sub(struct trie_node * node_i);
/*checking for each node in the trie if the str exists in the structure.*/
static trie_node *trie_exists_sub(struct trie_node * node_i,const char * str);
/* traveling through the trie tree and calling the print_func for each word in the tree */
static void trie_travel_internal(trie_node *node,void (*print_func)(data_trie *ptr),data_trie * ptr);

/****************************************************************************************************************/
/**************************************Core functions of the Trie structure**************************************/
/****************************************************************************************************************/

// handling memory allocation errors is done outside this function.
Trie create_trie() {
    return (Trie) calloc(1,sizeof(struct trie));
}

const char *trie_insert(Trie trie,const char *str,int line_num) {
    const char * temp = str;
    trie_node **node_ptr;

    // if the string is empty or it's NULL, there's no point in inserting it into the trie
    if(!str || *str == '\0' || isEmptyString((char*) str)) return NULL;

    node_ptr = &trie->children[(*str) - TRIE_START_CHAR]; /* the 'node' that travels along the tree and assigns values*/
    while(1) {
        if(*node_ptr == NULL) {
            (*node_ptr) = (trie_node *) calloc(1,sizeof(struct trie_node));
            if(*node_ptr == NULL){
                trie_full_delete(&trie); /* if the allocation failed, ABORT*/
                return "memory allocation error";
            }
        }
        str++;
        if(*str !='\0') node_ptr = &(*node_ptr)->children[(*str) - TRIE_START_CHAR];
        else break;
    }
    /* adding the proper data to the node to save the data of the string it represents*/
    trie_update_node(node_ptr,temp,line_num); 
    
    return temp;
}

void trie_delete(Trie trie,const char *str,int line_num) {
    struct trie_node * find_node;

    /* if the string is empty or it's NULL, there's no point in deleting it from the trie*/
    if(!str || *str == '\0' || isEmptyString((char*) str)) return;

    find_node = trie_exists_sub(trie->children[(*str) - TRIE_START_CHAR],str+1);
    if(find_node){ /* if the string was found in the trie, then it's data is erased to make
                      it an ordinary node and not a one representing a valid string */
        free((*find_node).symbol_data.symbol_name);
        (*find_node).symbol_data.symbol_name = NULL;
        (*find_node).symbol_data.word_count--;
        (*find_node).symbol_data.line_appears = NULL;
    } 

    return;
}

char* trie_check_exists(Trie trie,const char *str,int line_num) {
    trie_node *find_node;

    /* if there are no characters to process in the string, no point in doing any checking*/
    if(!str || *str == '\0' || isEmptyString((char*) str)) return NULL;

    find_node = trie_exists_sub(trie->children[(*str) - TRIE_START_CHAR],str+1);
    if(find_node != NULL){
        trie_update_node(&find_node,str,line_num);
    }
    return find_node == NULL ? NULL : (*find_node).symbol_data.symbol_name;
}

int trie_update_node(trie_node **node,const char *str,int line_num){
    data_trie *temp;
    temp = &(*node)->symbol_data;

    /* if the node is not representing a valid string, then update it with all the fields in it*/
    if((*temp).symbol_name == NULL){
        (*temp).symbol_name = (char *) calloc(strlen(str)+1,sizeof(char));
        if((*temp).symbol_name == NULL) return 0; /* if the allocation failed, ABORT*/
        strcpy((*temp).symbol_name,(char*) str);

        (*temp).ext_count++;
    }
    /* if the node is already representing a valid string, then only update if it appears in a different line then before*/
    else{

        (*temp).word_count++;
    }
    return 1;
}

void trie_travel(Trie trie, void (*print_func)(data_trie *ptr),data_trie * ptr) {
    int i;
    for(i = 0 ; i < ALPHABET_SIZE ; i++) {
        if(trie->children[i]) {
            trie_travel_internal(trie->children[i],print_func,ptr);
        }
    }
    return;
}

void trie_full_delete(Trie * trie) {
    int i;
    if(*trie != NULL) {
        Trie t = *trie;
        for(i = 0 ; i < ALPHABET_SIZE ; i++) {
            if(t->children[i] != NULL) trie_destroy_sub(t->children[i]);
        }
        free(*trie);
        (*trie) = NULL;
    }
    return;
}

/****************************************************************************************************************/
/**************************************Static internal functions for overload************************************/
/****************************************************************************************************************/

/* checks if a specific node represents a string */
static trie_node *trie_exists_sub(trie_node * node_i,const char * str) {
    while(node_i) {
        if(*str == '\0') {
            if((*node_i).symbol_data.symbol_name != NULL) {
                return node_i;
            }
            return NULL;
        }
        node_i = node_i->children[(*str) - TRIE_START_CHAR];
        str++;
    }
    return NULL;
}

static void trie_travel_internal(trie_node *node,void (*print_func)(data_trie *ptr),data_trie * ptr)  {
    int i;
    if((*node).symbol_data.symbol_name != NULL) {
        print_func(&(*node).symbol_data);
    }
    for(i = 0 ; i < ALPHABET_SIZE ; i++) {
        if(node->children[i]) {
            trie_travel_internal(node->children[i],print_func,ptr);
        }
    }
    return;
}

static void trie_destroy_sub(struct trie_node * node_i) {
    int i;
    for(i = 0 ; i < ALPHABET_SIZE ; i++) {
        if(node_i->children[i] != NULL) {
            trie_destroy_sub(node_i->children[i]);
            node_i->children[i] = NULL;
        }
    }
    free(node_i);
    return;
}

/*the function that prints the content of the trie node (presented as ptr)
prints the */
/*
void my_print_func(data_trie *ptr) {
    int i;
    int num,*arr_num;
    char c,aux_str[6];

    // assigning the struct members to local variables for better visibility
    char *word = (*ptr).symbol_name;
    num = (*ptr).word_count;
    arr_num = (*ptr).line_appears;

    if(num > 1) strcpy(aux_str,"lines");
    else        strcpy(aux_str,"line");

    printf("%-7s appears in %s ",word,aux_str);
    for(i = 0 ; i < num ; i++) {
        // if the line number is the same as the previous one, don't print it because we only need to show in which line it appeared
        if(i > 0 && arr_num[i] == arr_num[i-1]) continue;
        c = (i == num-1) ? '\n' : ',';
        printf("%d%c",arr_num[i],c);
    }
    
    return;
}
*/