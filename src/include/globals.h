/****************************************************/
/* File: globals.h                                  */
/* Global types and vars for TINY compiler          */
/* must come before other include files             */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <ctype.h>
#include <stdbool.h> //Using `stdbool.h` instead of `TRUE` and `FALSE`
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 10 // Updated for new changes

typedef enum
/* book-keeping tokens */
{ ENDFILE,
  ERROR,
  /* reserved words */
  IF,
  THEN,
  ELSE,
  ENDIF,
  ENDWHILE,
  REPEAT,
  UNTIL,
  READ,
  WRITE,
  WHILE,
  /* multicharacter tokens */
  ID,
  NUM,
  /* special symbols */
  ASSIGN,
  EQ,
  LT,
  PLUS,
  MINUS,
  TIMES,
  OVER,
  LPAREN,
  RPAREN,
  SEMI,
  DDOT,
} TokenType;

extern FILE* source;  /* source code text file */
extern FILE* listing; /* listing output text file */
extern FILE* code;    /* code text file for TM simulator */

extern int lineno; /* source line number for listing */

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

typedef enum { StmtK, ExpK } NodeKind;
typedef enum {
    IfK,
    RepeatK,
    AssignK,
    ReadK,
    WriteK,
    WhileK,
    SwitchK,
    CaseK
} StmtKind;
typedef enum { OpK, ConstK, IdK } ExpKind;

/* ExpType is used for type checking */
typedef enum { Void, Integer, Boolean } ExpType;

#define MAXCHILDREN 3

typedef struct treeNode {
    struct treeNode* child[MAXCHILDREN];
    struct treeNode* sibling;
    int lineno;
    NodeKind nodekind;
    union {
        StmtKind stmt;
        ExpKind exp;
    } kind;
    union {
        TokenType op;
        int val;
        char* name;
    } attr;
    ExpType type; /* for type checking of exps */
} TreeNode;

/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/

/* EchoSource = true causes the source program to
 * be echoed to the listing file with line numbers
 * during parsing
 */
extern bool EchoSource;

/* TraceScan = true causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern bool TraceScan;

/* TraceParse = true causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children)
 */
extern bool TraceParse;

/* TraceAnalyze = true causes symbol table inserts
 * and lookups to be reported to the listing file
 */
extern bool TraceAnalyze;

/* TraceCode = true causes comments to be written
 * to the TM code file as code is generated
 */
extern bool TraceCode;

/* Error = true prevents further passes if an error occurs */
extern bool Error;

#endif