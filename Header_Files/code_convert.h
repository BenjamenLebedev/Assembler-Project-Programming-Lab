 
#include "vector_lib.h"
#include "global_var.h"

#include "t_unit.h"

/**
 * @brief converts an integer number into a secret base representation
 * 
 * The function takes an integer number and converts it into a secret base 4 representation,
 *  using the characters '*', '#', '%', and '!' to represent the digits 0,1,2,3
 * 
 * @param number the integer number to be converted
 * @param secretBase an array of characters where the secret base representation will be stored
 */ 
void convertToSecretBase(int number, char secretBase[]);

/**
 * @brief creates an object file for a given translation unit.
 * the function creates an object file using the provided file name,
 * writing the code and data images of the translation unit into the file.
 * 
 * @param translation_unit the translation unit containing code and data images.
 * @param FileName the file name, for creating the object file.
 * @return Returns TRUE if there was an error creating the file, otherwise FALSE.
 */
int make_ob_file(struct translation_unit *translation_unit, char *FileName);

/**
 * @brief creates an extern file for a given translation unit.
 * the function creates an external file using the provided file name,
 * writing the external symbols and their addresses into the file
 * 
 * @param translation_unit the translation unit containing the external symbols.
 * @param FileName the file name, for creating the external file.
 * @return returns TRUE if there was an error creating the file, otherwise FALSE.
 */
int make_extern_file(struct translation_unit *translation_unit, char *FileName);

/**
 * @brief creates an entries file for the provided translation unit.
 * the function creates an entries file using the provided file name,
 * writing the entry symbols and their addresses into the file if any entry symbols exist in the translation unit
 * 
 * @param translation_unit the translation unit containing entry symbols.
 * @param FileName the base file name to be used for creating the entries file.
 * @return returns TRUE (non-zero) if there was an error creating the file, otherwise FALSE (zero).
 */
int make_entries_file(struct translation_unit *translation_unit, char *FileName);

/**
 * @brief generates the output files for a given translation unit.
 * the function generates the object file .ob, external file .ext, and entries file .ent for the provided translation unit.
 * 
 * @param translation_unit the translation unit containing the data to be written to the files.
 * @param FileName the base file name to be used for creating the output files.
 * @return returns TRUE if there was an error creating any of the files, otherwise FALSE.
 */
int gen_output_files(struct translation_unit *translation_unit, char *FileName);

/****************************************************************************/
/**************functions for sorting the symbol(entry) vector****************/
/****************************************************************************/

/**
 * @brief sorts the symbols vector by the address of the symbols.
 * 
 * The function sorts the symbols vector by the address of the symbols in ascending order.
 * 
 * @param translation_unit the translation unit containing the symbols vector to be sorted.
 * @return int returns TRUE if there was an error sorting the vector, otherwise FALSE.
 */
int vector_sort_merge(struct translation_unit *translation_unit);

/*recursive merge sort*/
void mergeSort(void **arr, int n, int *is_error);
/*the merge itself - done by comparing the addresses of the entry symbols*/
void merge(void **arr, void **l, int leftCount, void **r, int rightCount);