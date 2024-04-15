#include "../Header_Files/global_var.h"
#include "../Header_Files/vector.h"


#define VECTOR_BEGIN_SIZE 12

struct vector {
    void ** items;
    int  pointer_count;
    int  item_count;
    void *  (*ctor)(const void *copy);
    void    (*dtor)(void *item);
};

Vector new_vector(void * (*ctor)(const void *copy),void (*dtor)(void *item)) {
    Vector newVec = (Vector) calloc(1,sizeof(struct vector));
    if(newVec == NULL)
        return NULL;
    newVec->pointer_count = VECTOR_BEGIN_SIZE;
    newVec->items = (void**) calloc(VECTOR_BEGIN_SIZE,sizeof(void*));
    if(newVec->items == NULL) { /*memory allocation failed*/
        free(newVec);
        return NULL;
    }
    newVec->ctor = ctor;
    newVec->dtor = dtor;
    return newVec;
}

void * vector_insert(Vector v,const void * copy_item) {
    int it;
    void ** temp;
    if(v->item_count == v->pointer_count) {
        v->pointer_count *= 2;
        temp = (void**) realloc(v->items,v->pointer_count * sizeof(void *));
        if(temp == NULL) { /*memory allocation failed - we reduce back the size and abort*/
            v->pointer_count /= 2;
            return NULL;
        }
        v->items = temp;
        v->items[v->item_count] = v->ctor(copy_item);
        if(v->items[v->item_count] ==NULL) {
            return NULL;
        }
        v->item_count++;
        for(it = v->item_count; it < v->pointer_count; it++) {
            v->items[it] = NULL;
        }
    }else {
        for(it = 0;it<v->pointer_count;it++) {
            if(v->items[it] == NULL) {
                v->items[it] = v->ctor(copy_item);
                if(v->items[it] != NULL) {
                    v->item_count++;
                    break;    
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

int vector_get_item_count(const Vector v) {
    return v->item_count; 
}

void vector_destroy(Vector * v) {
    int it;
    if(*v != NULL) {
        if((*v)->dtor != NULL) {
            for(it = 0;it < (*v)->pointer_count; it++) {
                if((*v)->items[it] != NULL)
                    (*v)->dtor((*v)->items[it]);
            }
        }
        free((*v)->items);
        free(*v);
        *v = NULL;
    }
}