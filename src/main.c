/****************************************************/
/* File: main.c                                     */
/* Main program for TINY compiler                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "include/globals.h"

/* set NO_PARSE to 1 to get a scanner-only compiler */
#define NO_PARSE false
/* set NO_ANALYZE to 1 to get a parser-only compiler */
#define NO_ANALYZE false
/* set NO_CODE to TRUE to get a compiler that does not
 * generate code
 */
#define NO_CODE false

#include "include/util.h"
#if NO_PARSE
#include "include/scan.h"
#else
#include "include/parse.h"
#if !NO_ANALYZE
#include "include/analyze.h"
#if !NO_CODE
#include "include/cgen.h"
#endif
#endif
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
#if !NO_ANALYZE
    if (!Error) {
        if (TraceAnalyze) {
            fprintf(listing, "\nBuilding Symbol Table...\n");
        }
        buildSymtab(syntaxTree);
        if (TraceAnalyze) {
            fprintf(listing, "\nChecking Types...\n");
        }
        typeCheck(syntaxTree);
        if (TraceAnalyze) {
            fprintf(listing, "\nType Checking Finished\n");
        }
    }
#if !NO_CODE
    if (!Error) {
        char* codefile;
        int fnlen = strcspn(pgm, ".");
        codefile = (char*)calloc(fnlen + 4, sizeof(char));
        strncpy(codefile, pgm, fnlen);
        strcat(codefile, ".tm");
        code = fopen(codefile, "w");
        if (code == NULL) {
            printf("Unable to open %s\n", codefile);
            exit(1);
        }
        codeGen(syntaxTree, codefile);
        fclose(code);
    }
#endif
#endif
#endif
    freeTree(syntaxTree);
    return EXIT_SUCCESS;
}