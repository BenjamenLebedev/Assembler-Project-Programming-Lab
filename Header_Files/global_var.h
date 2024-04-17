#ifndef GLOBAL_VAR_H
#define GLOBAL_VAR_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h> 
#include <stdarg.h>
#include <stddef.h>

enum {
    FALSE,
    TRUE
};

/* Maximal length for a single line of command*/
#define MAX_LINE_LEN 82

/* Maximal length of a label */
#define MAX_LABEL_LEN 31

/* length of each word in command line, in bits */
#define WORD_LEN 14

/* Initial length for error string */
#define INIT_ERROR_LEN 300

/* Total number of registers allowed in the system*/
#define REG_NUM 8

/* the number of instruction operation codes */
#define OPCODE_NUM 16

/* the number of legal directives for the assembler */
#define DIR_NUM 5

/*The starting value of the instruction counter*/
#define IC_START 100

/* the maximal and minimal values of integer in the instructions for the fictional machine 
in the instructions we have the first 2 bits reserved (A.R.E) so here we go with only 12 bits
meaning 12-1 = 14-3 --> since we support negative integers we need the last bit for sign representation*/
#define MAX_INT_INST ((1 << (WORD_LEN-3)) - 1) /*2047*/
#define MIN_INT_INST -(1 << (WORD_LEN-3)) /*-2048*/

/* the maximal and minimal values of integer in the directives for the fictional machine 
in the directives we use all 14 bits for the integer so the values are like this*/
#define MAX_INT_DIR ((1 << (WORD_LEN-1)) - 1) /*8191*/
#define MIN_INT_DIR -(1 << (WORD_LEN-1)) /*-8192*/

/* the maximal amount of offset - since in .data directive we can't have more than
37 numbers at most (single digit numbers) - since the line is confined to 82 characters (including newline character)*/
#define MAX_OFFSET 36


#endif