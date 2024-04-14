 
#include "vector.h"
#include "global_var.h"

#include "t_unit.h"

void convertToSecretBase(int number, char secretBase[]);

int make_ob_file(const struct translation_unit *translation_unit, char *FileName);

int make_extern_file(const struct translation_unit *translation_unit, char *FileName);

int make_entries_file(const struct translation_unit *translation_unit, char *FileName);