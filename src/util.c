/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "util.h"

/* Procedure printToken prints a token
 * and its lexeme to the listing file
 */
void printToken(TokenType token, const char* tokenString)
{
    switch (token) {
    case IF:
    case THEN:
    case ELSE:
    case ENDIF:
    case ENDWHILE:
    case REPEAT:
    case UNTIL:
    case READ:
    case WRITE:
    case WHILE:
        fprintf(listing, "reserved word: %s\n", tokenString);
        break;
    case ASSIGN:
        fprintf(listing, ":=\n");
        break;
    case LT:
        fprintf(listing, "<\n");
        break;
    case EQ:
        fprintf(listing, "=\n");
        break;
    case LPAREN:
        fprintf(listing, "(\n");
        break;
    case RPAREN:
        fprintf(listing, ")\n");
        break;
    case SEMI:
        fprintf(listing, ";\n");
        break;
    case PLUS:
        fprintf(listing, "+\n");
        break;
    case MINUS:
        fprintf(listing, "-\n");
        break;
    case TIMES:
        fprintf(listing, "*\n");
        break;
    case DDOT:
        fprintf(listing, ":\n");
        break;
    case OVER:
        fprintf(listing, "/\n");
        break;
    case ENDFILE:
        fprintf(listing, "EOF\n");
        break;
    case NUM:
        fprintf(listing, "NUM, val= %s\n", tokenString);
        break;
    case ID:
        fprintf(listing, "ID, name= %s\n", tokenString);
        break;
    case ERROR:
        fprintf(listing, "ERROR: %s\n", tokenString);
        break;
    default: /* should never happen */
        fprintf(listing, "Unknown token: %d\n", token);
    }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode* newStmtNode(StmtKind kind)
{
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (node == NULL) {
        fprintf(listing, "Out of memory error at line %d\n", lineno);
        return NULL;
    }
    for (int i = 0; i < MAXCHILDREN; i++) {
        node->child[i] = NULL;
    }
    node->sibling = NULL;
    node->nodekind = StmtK;
    node->kind.stmt = kind;
    node->lineno = lineno;
    return node;
}

/* Function newExpNode creates a new expression
 * node for syntax tree construction
 */
TreeNode* newExpNode(ExpKind kind)
{
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (node == NULL) {
        fprintf(listing, "Out of memory error at line %d\n", lineno);
        return NULL;
    }
    for (int i = 0; i < MAXCHILDREN; i++) {
        node->child[i] = NULL;
    }
    node->sibling = NULL;
    node->nodekind = ExpK;
    node->kind.exp = kind;
    node->lineno = lineno;
    node->type = Void;
    return node;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char* copyString(char* s)
{
    int n;
    char* t;
    if (s == NULL) {
        return NULL;
    }
    n = strlen(s) + 1;
    t = malloc(n);
    if (t == NULL) {
        fprintf(listing, "Out of memory error at line %d\n", lineno);
    }
    else {
        strcpy(t, s);
    }
    return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno += 2
#define UNINDENT indentno -= 2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{
    for (int i = 0; i < indentno; i++) {
        fprintf(listing, " ");
    }
}

/* procedure printTree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
void printTree(TreeNode* tree)
{
    INDENT;
    while (tree != NULL) {
        printSpaces();
        if (tree->nodekind == StmtK) {
            switch (tree->kind.stmt) {
            case IfK:
                fprintf(listing, "If\n");
                break;
            case RepeatK:
                fprintf(listing, "Repeat\n");
                break;
            case AssignK:
                fprintf(listing, "Assign to: %s\n", tree->attr.name);
                break;
            case ReadK:
                fprintf(listing, "Read: %s\n", tree->attr.name);
                break;
            case WriteK:
                fprintf(listing, "Write\n");
                break;
            case WhileK:
                fprintf(listing, "While\n");
                break;
            default:
                fprintf(listing, "Unknown ExpNode kind\n");
                break;
            }
        }
        else if (tree->nodekind == ExpK) {
            switch (tree->kind.exp) {
            case OpK:
                fprintf(listing, "Op: ");
                printToken(tree->attr.op, "\0");
                break;
            case ConstK:
                fprintf(listing, "Const: %d\n", tree->attr.val);
                break;
            case IdK:
                fprintf(listing, "Id: %s\n", tree->attr.name);
                break;
            default:
                fprintf(listing, "Unknown ExpNode kind\n");
                break;
            }
        }
        else {
            fprintf(listing, "Unknown node kind\n");
        }
        for (int i = 0; i < MAXCHILDREN; i++) {
            printTree(tree->child[i]);
        }
        tree = tree->sibling;
    }
    UNINDENT;
}

void freeTree(TreeNode* tree)
{
    while (tree) {
        for (int i = 0; i < MAXCHILDREN; i++) {
            free(tree->child[i]);
        }
        tree = tree->sibling;
    }
    if (tree) {
        free(tree);
    }
}
