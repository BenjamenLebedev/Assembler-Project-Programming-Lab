 
#include "vector.h"
#include "global_var.h"

#include "t_unit.h"

void convertToSecretBase(int number, char secretBase[]);

int make_ob_file(const struct translation_unit *translation_unit, char *FileName);

int make_extern_file(const struct translation_unit *translation_unit, char *FileName);

int make_entries_file(struct translation_unit *translation_unit, char *FileName);

/****************************************************************************/
/**************functions for sorting the symbol(enrty) vector****************/
/****************************************************************************/

void vector_sort(struct translation_unit *translation_unit);

void vector_sort_merge(struct translation_unit *translation_unit);
void mergeSort(void **arr, int n);
void merge(void **arr, void **l, int leftCount, void **r, int rightCount);