/****************************************************/
/* File: main.c                                     */
/* Main program for TINY compiler                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"

/* set NO_PARSE to 1 to get a scanner-only compiler */
#define NO_PARSE false
/* set NO_ANALYZE to 1 to get a parser-only compiler */
#define NO_ANALYZE false

/* set NO_CODE to TRUE to get a compiler that does not
 * generate code
 */
#define NO_CODE false

#include "util.h"

/* allocate global variables */
int lineno = 0;
FILE* source;
FILE* listing;
FILE* code;

/* allocate and set tracing flags */
bool EchoSource = true;
bool TraceScan = true;
bool TraceParse = true;
bool TraceAnalyze = true;
bool TraceCode = true;

bool Error = false;

int main()
{
    printf("Hello, World!\n");
    return 0;
}