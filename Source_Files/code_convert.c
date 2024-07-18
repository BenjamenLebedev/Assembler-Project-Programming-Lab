
#include "../Header_Files/vector_lib.h"
#include "../Header_Files/firstSecond_Pass.h"
#include "../Header_Files/global_var.h"
#include "../Header_Files/t_unit.h"
#include "../Header_Files/code_convert.h"


void convertToSecretBase(int number, char secretBase[]) {

    int i; 
    int mask,pair;

    /* the secret base - encrypted base 4*/
    /*    0    1    2    3     */
    /*    *    #    %    !     */

    /* initialize the array with '*' for positive and '!' for negative */ 
    for (i = 0; i < WORD_LEN/2; i++) {
        if(number < 0) secretBase[i] = '!';
        else secretBase[i] = '*';
    }
    secretBase[WORD_LEN/2] = '\0'; 

    /* convert the number to secret base */
    i = WORD_LEN/2 - 1; /* start from the end of the array */
    while ((number > 0 || number < 0) && i >= 0) { 
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

int gen_output_files(struct translation_unit *translation_unit, char *FileName){
    int is_error = FALSE;
    
    if(make_ob_file(translation_unit, FileName)){
        is_error = TRUE;
    }

    if(make_extern_file(translation_unit, FileName)){
        is_error = TRUE;
    }

    if(make_entries_file(translation_unit, FileName)){
        is_error = TRUE;
    }
    
    return is_error;
}

int make_ob_file(struct translation_unit *translation_unit, char *FileName){
    int i,total;
    char secretBase[8];
    int is_error = FALSE;

    char * file_ob_name;
    FILE *file_ob;
    char *ob_extension = ".ob";
    char newline[2] = "\n";

    file_ob_name = (char *)malloc(strlen(FileName) + + strlen(ob_extension) + 1);
    if(!file_ob_name){
        printf("********** error: Memory allocation error\n");
        free_translation_unit(translation_unit);
        return is_error = TRUE;
    }
    strcpy(file_ob_name, FileName);
    strcat(file_ob_name, ob_extension);
    file_ob = fopen(file_ob_name, "w"); /*create file*/
    if(!file_ob){
        printf("********** error: cannot open file %s for writing\n", file_ob_name);
    }

    if(file_ob){
        fprintf(file_ob, "  %d %d\n", translation_unit->IC, translation_unit->DC);
        /*first printing the code image*/
        for(total = i = 0; i<translation_unit->IC; i++, total++){
            convertToSecretBase(translation_unit->code_image[i], secretBase);
            fprintf(file_ob, "0%d %s\n",total+100, secretBase);
        }
        /*now printing the data image*/
        for(i = 0; i<translation_unit->DC; i++, total++){
            convertToSecretBase(translation_unit->data_image[i], secretBase);
            if(i == translation_unit->DC - 1) strcpy(newline, "");
            
            fprintf(file_ob, "0%d %s%s",total+100 ,secretBase,newline);
        }
    }
    else{
        is_error = TRUE;
    }
    fclose(file_ob);
    free(file_ob_name);

    return is_error;
}


int make_extern_file(struct translation_unit *translation_unit, char *FileName){

    int i,is_error = FALSE;
    void *const *begin;
    void *const *end;
    struct ext *external;

    char * file_ext_name;
    FILE *file_ext = NULL;
    char *ext_extension = ".ext";
    

    file_ext_name = (char *)malloc(strlen(FileName) + strlen(ext_extension) + 1);
    if(!file_ext_name){
        printf("********** error: Memory allocation error\n");
        free_translation_unit(translation_unit);
    }
    strcpy(file_ext_name, FileName);
    strcat(file_ext_name, ext_extension);


    /*only open file if there are any external symbols that are used in the assembly code*/
    external = (struct ext *) vector_begin(translation_unit->externals); 
    if(external && translation_unit->extern_use == TRUE){
        file_ext = fopen(file_ext_name, "w"); /*create file*/
        if(!file_ext){
            printf("********** error: cannot open file %s for writing\n", file_ext_name);
            is_error = TRUE;
        }
    }
    if(file_ext){
        VECTOR_LOOP(begin, end, translation_unit->externals) {
            if (*begin) {
                external = (struct ext *) *begin;

                for(i=0; i<external->address_count; i++){
                    fprintf(file_ext, "%s\t0%d\n", external->ext_name, external->address[i]);
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

    
    file_ent_name = (char *)malloc(strlen(FileName) + strlen(ent_extension) + 1);
    if(!file_ent_name){
        printf("********** error: Memory allocation error\n");
        free_translation_unit(translation_unit);
        exit(1);
    }
    strcpy(file_ent_name, FileName);
    strcat(file_ent_name, ent_extension);

    /* creating file only if an entry symbol is used somewhere*/
    entrie = (struct symbol *) vector_begin(translation_unit->symbols);
    if(entrie && translation_unit->entry_use == TRUE && !is_error){
        file_ent = fopen(file_ent_name, "w"); /*create file*/
        if(!file_ent){
            printf("********** error: cannot open file %s for writing\n", file_ent_name);
            is_error = TRUE;
        }
        /*sorting the symbol vector by increasing addresses*/
        /*vector_sort(translation_unit);*/
        if(!is_error){
            is_error = vector_sort_merge(translation_unit);
            if(is_error){
                printf("********** error: memory allocation error during sorting of entry symbols vector\n");
                free_translation_unit(translation_unit);
                free(file_ent_name);
                fclose(file_ent);
                exit(1);
            }
        }
    }
    if(file_ent && !is_error){
        VECTOR_LOOP(begin, end, translation_unit->symbols) {
            if (*begin) {
                entrie = (struct symbol *) *begin;
                if(entrie->symType == entryDataSymbol || entrie->symType == entryCodeSymbol ){
                    fprintf(file_ent, "%s\t0%d\n", entrie->symName, entrie->address);
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


int vector_sort_merge(struct translation_unit *translation_unit){

    int vecSize;
    
    int *is_error,x;
    is_error = &x;
    vecSize = vector_length(translation_unit->symbols);
    *is_error = FALSE;
    mergeSort((void **) vector_begin(translation_unit->symbols), vecSize,is_error);
    
    return *is_error;
}


void mergeSort(void **arr, int n, int *is_error){

    int i,mid;
    void **l, **r;

    if(*is_error == TRUE){
        return;
    }

    if(n < 2) return;
    mid = n/2;
    l = (void **) malloc(mid * sizeof(void *));
    r = (void **) malloc((n-mid) * sizeof(void *));
    if(l == NULL || r == NULL){
        printf("********** error: Memory allocation error\n");
        *is_error = TRUE;
        return;
    }

    
    for(i = 0; i < mid; i++){
        l[i] = arr[i];
    }
    for(i = mid; i < n; i++){
        r[i-mid] = arr[i];
    }
    mergeSort(l, mid, is_error);
    mergeSort(r, n-mid, is_error);
    
    if(*is_error == TRUE){
        if(l) free(l);
        if(r) free(r);
        return;
    }
    
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