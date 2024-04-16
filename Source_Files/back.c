
#include "../Header_Files/vector.h"
#include "../Header_Files/mid.h"
#include "../Header_Files/global_var.h"
#include "../Header_Files/t_unit.h"
#include "../Header_Files/back.h"


void convertToSecretBase(int number, char secretBase[]) {

    int i; 
    int mask,pair;

    /* the secret base - encrypted base 4*/
    /*    0    1    2    3     */
    /*    *    #    %    !     */

    /* initialize the array with '*' */ 
    if (number < 0) {
        /* number = -number - 1;  */
        for (i = 0; i < 7; i++) {
            secretBase[i] = '!';
        }
    } else {
        for (i = 0; i < 7; i++) {
            secretBase[i] = '*';
        }
    }
    secretBase[7] = '\0'; 

    /* convert the number to secret base */
    i = 6; /* start from the end of the array */
    while (number > 0 && i >= 0) {
        mask = 3;
        pair = number & mask;
        switch (pair) {
            case 0:
                secretBase[i--] = '*';
                break;
            case 1:
                secretBase[i--] = '#';
                break;
            case 2:
                secretBase[i--] = '%';
                break;
            case 3:
                secretBase[i--] = '!';
                break;
        }
        number >>= 2;
    }
    while (number < 0 && i >= 0) {
        mask = 3;
        pair = number & mask;
        switch (pair) {
            case 0:
                secretBase[i--] = '*';
                break;
            case 1:
                secretBase[i--] = '#';
                break;
            case 2:
                secretBase[i--] = '%';
                break;
            case 3:
                secretBase[i--] = '!';
                break;
        }
        number >>= 2;
    }

}

/***********************************************************************************/
/***********************functions to print object files*****************************/
/***********************************************************************************/

int make_ob_file(const struct translation_unit *translation_unit, char *FileName){
    int i,total;
    char secretBase[8];
    int is_error = FALSE;

    char * file_ob_name;
    FILE *file_ob;
    char *ob_extension = ".ob";

    file_ob_name = (char *)malloc(strlen(FileName) + + strlen(ob_extension) + 1);
    strcpy(file_ob_name, FileName);
    strcat(file_ob_name, ob_extension);
    file_ob = fopen(file_ob_name, "w"); /*create file*/ 

    if(file_ob){
        fprintf(file_ob, "  %d %d\n", translation_unit->IC, translation_unit->DC);
        /*first printing the code image*/
        for(total = i = 0; i<translation_unit->IC; i++, total++){
            convertToSecretBase(translation_unit->code_image[i], secretBase);
            printf("IC %d: %s\n",i+100, secretBase);
            fprintf(file_ob, "%04d %s  (code image: %d)\n",total+100, secretBase, translation_unit->code_image[i]);
        }
        /*now printing the data image*/
        for(i = 0; i<translation_unit->DC; i++, total++){
            convertToSecretBase(translation_unit->data_image[i], secretBase);
            printf("DC: %s\n", secretBase);
            fprintf(file_ob, "%04d %s  (data image: %d)\n",total+100 ,secretBase, translation_unit->data_image[i]);
        }
    }
    else{
        is_error = TRUE;
    }
    fclose(file_ob);
    free(file_ob_name);

    return is_error;

}


int make_extern_file(const struct translation_unit *translation_unit, char *FileName){

    int i,is_error = FALSE;
    void *const *begin;
    void *const *end;
    struct ext *external;

    char * file_ext_name;
    FILE *file_ext = NULL;
    char *ext_extension = ".ext";

    file_ext_name = (char *)malloc(strlen(FileName) + + strlen(ext_extension) + 1);
    if(!file_ext_name){
        printf("********* Memory allocation error\n");
        is_error = TRUE;
    }
    strcpy(file_ext_name, FileName);
    strcat(file_ext_name, ext_extension);


    /*only open file if there are any external symbols that are used in the assembly code*/
    external = (struct ext *) vector_begin(translation_unit->externals); 
    if(external && translation_unit->extern_use == TRUE){
        file_ext = fopen(file_ext_name, "w"); /*create file*/
        if(!file_ext){
            printf("********* Error: cannot open file %s for writing\n", file_ext_name);
            is_error = TRUE;
        }
    }
    if(file_ext){
        printf("in extern if(file_ext) func\n");
        VECTOR_FOR_EACH(begin, end, translation_unit->externals) {
            if (*begin) {
                external = (struct ext *) *begin;
                printf("********* extern: %s, count: %d\n", external->ext_name, external->address_count); 

                for(i=0; i<external->address_count; i++){
                    fprintf(file_ext, "%s\t%04d\n", external->ext_name, external->address[i]);
                }
            }
        } /*end iterating on vector*/
    }
    if(file_ext) fclose(file_ext);
    free(file_ext_name);

    return is_error;

}

int make_entries_file(struct translation_unit *translation_unit, char *FileName){\
    int is_error = FALSE;
    void *const *begin;
    void *const *end;
    struct symbol *entrie;

    char * file_ent_name;
    FILE *file_ent = NULL;
    char *ent_extension = ".ent";

    
    file_ent_name = (char *)malloc(strlen(FileName) + + strlen(ent_extension) + 1);
    if(!file_ent_name){
        printf("********* Memory allocation error\n");
        is_error = TRUE;
    }
    strcpy(file_ent_name, FileName);
    strcat(file_ent_name, ent_extension);

    /* creating file only if an entry symbol is used somewhere*/
    entrie = (struct symbol *) vector_begin(translation_unit->symbols);
    if(entrie && translation_unit->entry_use == TRUE){
        file_ent = fopen(file_ent_name, "w"); /*create file*/
        if(!file_ent){
            printf("********* Error: cannot open file %s for writing\n", file_ent_name);
            is_error = TRUE;
        }
        /*sorting the symbol vector by increasing addresses*/
        /*vector_sort(translation_unit);*/
        vector_sort_merge(translation_unit);
    }
    if(file_ent){
        printf("in extern if(file_ext) func\n");
        VECTOR_FOR_EACH(begin, end, translation_unit->symbols) {
            if (*begin) {
                entrie = (struct symbol *) *begin;   
                if(entrie->symType == entryDataSymbol || entrie->symType == entryCodeSymbol ){
                    printf("********* entry: %s\n", entrie->symName); 
                    fprintf(file_ent, "%s\t%04d\n", entrie->symName, entrie->address);
                }
            }
        }
    }
    if(file_ent) fclose(file_ent);
    free(file_ent_name);
    return is_error;
}


/****************************************************************************/
/**************functions for sorting the symbol(enrty) vector****************/
/****************************************************************************/

/*bubble sort*/
void vector_sort(struct translation_unit *translation_unit){

    void **begin_i,**begin_j;
    void **end_i,**end_j;
    struct symbol *temp;
    int i,j;
    int vecSize = vector_get_item_count(translation_unit->symbols);
    
    for(begin_i = (void **) vector_begin(translation_unit->symbols), end_i = (void **) vector_end(translation_unit->symbols),\
     i=0; begin_i < end_i && i < vecSize; begin_i++, i++){
        for(begin_j = (void **) vector_begin(translation_unit->symbols), end_j = (void **) vector_end(translation_unit->symbols), \
        j=0; begin_j < end_j && j < vecSize; begin_j++, j++){
            if(*begin_i && *begin_j){
                if(((struct symbol *)*begin_i)->address < ((struct symbol *)*begin_j)->address){
                    temp = (struct symbol *) *begin_i;
                    *begin_i = *begin_j;
                    *begin_j = temp;
                }
            }
        } /*end of begin_j loop*/
    } /*end of begin_i loop*/
}


/*using merge sort*/
void vector_sort_merge(struct translation_unit *translation_unit){

    int vecSize = vector_get_item_count(translation_unit->symbols);

    mergeSort((void **) vector_begin(translation_unit->symbols), vecSize);
}


void mergeSort(void **arr, int n){

    int i,mid;
    void **l, **r;

    if(n < 2) return;
    mid = n/2;
    l = (void **) calloc(mid, sizeof(void *));
    r = (void **) calloc(n-mid, sizeof(void *));

    
    for(i = 0; i < mid; i++){
        l[i] = arr[i];
    }
    for(i = mid; i < n; i++){
        r[i-mid] = arr[i];
    }
    mergeSort(l, mid);
    mergeSort(r, n-mid);
    merge(arr, l, mid, r, n-mid);
    free(l);
    free(r);
}


void merge(void **arr, void **l, int leftCount, void **r, int rightCount){
    int i,j,k;
    i = j = k = 0;
    while(i < leftCount && j < rightCount){
        if(((struct symbol *)l[i])->address < ((struct symbol *)r[j])->address){
            arr[k++] = l[i++];
        }else{
            arr[k++] = r[j++];
        }
    }
    while(i < leftCount){
        arr[k++] = l[i++];
    }
    while(j < rightCount){
        arr[k++] = r[j++];
    }
}