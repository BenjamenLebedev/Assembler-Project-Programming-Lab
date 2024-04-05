#define MAX_LINE_LENGTH 81

struct macro_information {
    char line[MAX_LINE_LENGTH];  
    struct macro_information* next;
};


struct macro {
    char name[MAX_LINE_LENGTH ];  
    struct macro_information* info_head;   /* pointer to Head of the macro information linked list */
    struct macro* next;   /* Pointer to the next macro */
};

enum {
    macro_definition,
    in_macro_definition,
    end_macro_definition,
    macro_call,
    regular_line
};

void addMacroInformation(struct macro* m, const char* line);
void addMacro(struct macro** macros, const char* name);
void displayMacro_information(const struct macro* macro);
void displayMacros(const struct macro* macros);
void freeMacro_information(struct macro_information* info);
void free_Macros(struct macro* macros);
char *trimSpaces(char *str);
struct macro *pointer_to_last_macro(struct macro** macros);
struct macro *is_string_macro(char *string, struct macro** original_macros);
int macro_line_classifier(char *line, struct macro** macros, struct macro** macro);
char *preprocessor(char *name);
