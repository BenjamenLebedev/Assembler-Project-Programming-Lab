#include "../Header_Files/global_var.h"
#include "../Header_Files/vector.h"


#define VECTOR_BEGIN_SIZE 15

 
Vector new_vector(void * (*ctor)(const void *copy),void (*dtor)(void *item)) {
    Vector newVec = (Vector) calloc(1,sizeof(struct vector));
    if(newVec == NULL)/*check if memory allocation failed*/
        return NULL;
    newVec->pointer_count = VECTOR_BEGIN_SIZE;
    newVec->items = (void**) calloc(VECTOR_BEGIN_SIZE,sizeof(void*));
    if(newVec->items == NULL) { /*check if memory allocation failed*/
        free(newVec);
        return NULL;
    }
    newVec->ctor = ctor; /* set the constructor function */
    newVec->dtor = dtor; /* set the destructor function */
    return newVec;
}

void * vector_insert(Vector v,const void * copy_item) {
    int it;
    void ** temp;
    if(v->item_count == v->pointer_count) {
        /* If the vector is full, double its size */
        v->pointer_count *= 2;
        temp = (void**) realloc(v->items,v->pointer_count * sizeof(void *));
        if(temp == NULL) { /*check if memory allocation failed*/
            v->pointer_count /= 2; /*restore original size*/
            return NULL;
        }
        v->items = temp;
        v->items[v->item_count] = v->ctor(copy_item); /*add new item with ctor*/
        if(v->items[v->item_count] ==NULL) {
            return NULL;
        }
        v->item_count++;
        for(it = v->item_count; it < v->pointer_count; it++) {
            v->items[it] = NULL;
        }
    }else {
        for(it = 0;it<v->pointer_count;it++) {
            if(v->items[it] == NULL) {  /*check if the current slot in the vectors array is empty*/
                v->items[it] = v->ctor(copy_item);
                if(v->items[it] != NULL) {
                    v->item_count++;
                    break;    /*break because we finished inserting*/
                }
                return NULL;
            }
        }
    }
    return v->items[v->item_count-1];
}


void *const *vector_begin(const Vector v) {
    return v->items;
}

void *const *vector_end(const Vector v) {
    return &v->items[v->pointer_count - 1];
}

int vector_length(const Vector v) {
    return v->item_count; 
}

void vector_destroy(Vector * v) {
    int it;
    if(*v != NULL) {
        if((*v)->dtor != NULL) {
            for(it = 0;it < (*v)->pointer_count; it++) {
                if((*v)->items[it] != NULL)
                    (*v)->dtor((*v)->items[it]); /*use the destruction func to free each item*/
            }
        }
        free((*v)->items); /*free items array*/
        free(*v);          /*free vec struct*/
        *v = NULL;   /*set vec pointer to null*/
    }
}