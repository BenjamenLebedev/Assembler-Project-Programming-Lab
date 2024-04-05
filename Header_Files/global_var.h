#ifndef GLOBAL_VAR_H
#define GLOBAL_VAR_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

enum {
    FALSE,
    TRUE
};

/* Maximal length for a single line of command*/
#define MAX_LINE_LEN 82

/* Maximal length of a label */
#define MAX_LABEL_LEN 31

/* length of each word in command line, in bits */
#define WORD_LEN_AST 14

/* Initial length for error string */
#define INIT_ERROR_LEN 200

/* Total number of registers allowed in the system*/
#define REG_NUM 8

/* the number of instruction operation codes */
#define OPCODE_NUM 16

/* the number of directives */
#define DIR_NUM 5

/* the maximal and minimal values of integer in the instructions for the fictional machine 
in the instructions we have the first 2 bits reserved (A.R.E) so here we go with only 12 bits
meaning 12-1 = 14-3 --> we shift only WORD_LEN_AST-1 since 1 has only WORD_LEN_AST-1 zero bits*/
#define MAX_INT_INST ((1 << (WORD_LEN_AST-3)) - 1) /*2047*/
#define MIN_INT_INST -(1 << (WORD_LEN_AST-3)) /*-2048*/

/* the maximal and minimal values of integer in the directives for the fictional machine 
in the directives we use all 14 bits for the integer so the values are like this*/
#define MAX_INT_DIR ((1 << (WORD_LEN_AST-1)) - 1) /*8191*/
#define MIN_INT_DIR -(1 << (WORD_LEN_AST-1)) /*-8192*/

/* the maximal amount of offset - since in .data directive we can't have more than
37 numbers at most (single digit numbers) - since the line is confined to 82 characters (including newline character)*/
#define MAX_OFFSET 36


#endif