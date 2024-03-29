/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "include/analyze.h"

/* counter for variable memory locations */
static int location = 0;

/* Procedure traverse is a generic recursive
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc
 * in postorder to tree pointed to by t
 */

static void traverse(TreeNode* t, void (*preProc)(TreeNode* t),
                     void (*postProc)(TreeNode* t))
{
    if (t == NULL) {
        return;
    }
    preProc(t);
    for (int i = 0; i < MAXCHILDREN; i++) {
        traverse(t->child[i], preProc, postProc);
    }
    postProc(t);
    traverse(t->sibling, preProc, postProc);
    return;
}

/* nullProc is a do-nothing procedure to
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc() { return; }

/* Procedure insertNode inserts
 * identifiers stored in t into
 * the symbol table
 */
static void insertNode(TreeNode* t)
{
    switch (t->nodekind) {
    case StmtK:
        switch (t->kind.stmt) {
        case AssignK:
        case ReadK:
            if (st_lookup(t->attr.name) == -1) {
                /* not yet in table, so treat as new definition */
                st_insert(t->attr.name, t->lineno, location++);
            }
            else {
                /* already in table, so ignore location,
                  add line number of use only */
                st_insert(t->attr.name, t->lineno, 0);
            }
            break;
        default:
            break;
        }
        break;
    case ExpK:
        switch (t->kind.exp) {
        case IdK:
            if (st_lookup(t->attr.name) == -1) {
                /* not yet in table, so treat as new definition */
                st_insert(t->attr.name, t->lineno, location++);
            }

            else {
                /* already in table, so ignore location,
                   add line number of use only */
                st_insert(t->attr.name, t->lineno, 0);
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

/* Function buildSymtab constructs the symbol
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode* tree)
{
    if (tree == NULL) {
        return;
    }
    traverse(tree, insertNode, nullProc);
    if (TraceAnalyze) {
        fprintf(listing, "\nSymbol table:\n\n");
        printSymTab(listing);
    }
}

static void typeError(TreeNode* t, char* message)
{
    fprintf(listing, "Type error at line %d: %s\n", t->lineno, message);
    Error = true;
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode* t)
{
    switch (t->nodekind) {
    case ExpK:
        switch (t->kind.exp) {
        case OpK:
            if ((t->child[0]->type != Integer) ||
                (t->child[1]->type != Integer)) {
                typeError(t, "Op applied to non-integer");
            }
            if ((t->attr.op == EQ) || (t->attr.op == LT)) {
                t->type = Boolean;
            }
            else {
                t->type = Integer;
            }
            break;
        case ConstK:
        case IdK:
            t->type = Integer;
            break;
        default:
            break;
        }
        break;
    case StmtK:
        switch (t->kind.stmt) {
        case IfK:
            if (t->child[0]->type == Integer) {
                typeError(t->child[0], "if test is not Boolean");
            }
            break;
        case AssignK:
            if (t->child[0]->type != Integer) {
                typeError(t->child[0], "assignment of non-integer value");
            }
            break;
        case WriteK:
            if (t->child[0]->type != Integer) {
                typeError(t->child[0], "write of non-integer value");
            }
            break;
        case RepeatK:
            if (t->child[1]->type == Integer) {
                typeError(t->child[1], "repeat test is not Boolean");
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

/* Procedure typeCheck performs type checking
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode* tree) { traverse(tree, nullProc, checkNode); }