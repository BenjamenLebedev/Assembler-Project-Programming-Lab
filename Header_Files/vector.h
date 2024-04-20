#ifndef __VECTOR_H_ 
#define __VECTOR_H_


struct vector {
    void ** items; /*pointer to array of items*/
    int  pointer_count;  /*number of pointers*/
    int  item_count;     /*number of actual items in the vector*/
    void *  (*ctor)(const void *copy);  /*construction func*/
    void    (*dtor)(void *item);        /*destruction func*/
};

typedef struct vector * Vector;

/**
 * @brief creates a new Vector .
 * 
 * @param ctor pointer to a constructor function.
 * @param dtor pointer to a destructor function.
 * @return the wanted vector
 */
Vector new_vector(void * (*ctor)(const void *copy),void (*dtor)(void *item));

 
/**
 * @brief inserts a new item to the vector
 * 
 * @param v the vector
 * @param copy_item pointer to the item you wish to push in the vector.
 * @return void* pointer to inserted element if successful, else returns NULL
 */
void * vector_insert(Vector v,const void * copy_item);


/**
 * @brief returns a pointer to the first pointer of the vector list
 * 
 * @param v reference to the vector
 * @return void* const* pointer to first pointer
 */
void *const *vector_begin(const Vector v);

/**
 * @brief returns a pointer to the last pointer of the vector item list.
 * 
 * @param v reference to the vector
 * @return void* const* pointer to last pointer in list 
 */
void *const *vector_end(const Vector v);

/**
 * @brief returns how many items(pointers) are actually inside the vector
 * 
 * @param v reference to the vector
 * @return int the length of the list
 */
int vector_length(const Vector v);

/**
 * @brief frees all the memory allocated for the vector
 * 
 * @param v reference to the vector
 */
void vector_destroy(Vector * v);


/**
 * @brief macro looping through the vector.
 * 
 */
#define VECTOR_LOOP(begin,end,v) for(begin   = vector_begin(v),end= vector_end(v);begin <= end;begin++)

#endif