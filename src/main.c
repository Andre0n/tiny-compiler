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
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#endif

/* allocate global variables */
int lineno = 0;
char* filePath;
FILE* source;
FILE* listing;
FILE* code;

/* allocate and set tracing flags */
bool EchoSource = false;
bool TraceScan = true;
bool TraceParse = true;
bool TraceAnalyze = true;
bool TraceCode = true;

bool Error = false;

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <filename.tny>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char pgm[120];
    strcpy(pgm, argv[1]);
    if (strchr(pgm, '.') == NULL) {
        strcat(pgm, ".tny");
    }
    source = fopen(pgm, "r");
    filePath = pgm;
    if (source == NULL) {
        fprintf(stderr, "File %s not found\n", pgm);
        exit(EXIT_FAILURE);
    }

    listing = stdout;
    fprintf(listing, "\nTINY COMPILATION: %s\n", pgm);
#if NO_PARSE
    while (getToken() != ENDFILE) {
        continue;
    }
#else
    TreeNode* syntaxTree;
    syntaxTree = parse();
    if (TraceParse && !Error) {
        fprintf(listing, "\nSyntax tree:\n");
        printTree(syntaxTree);
    }
    freeTree(syntaxTree);
#endif

    return EXIT_SUCCESS;
}