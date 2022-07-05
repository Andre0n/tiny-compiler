/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "include/parse.h"

static TokenType currentToken; /* holds current token */

/* function prototypes for recursive calls */
static TreeNode* stmt_sequence();
static TreeNode* statement();
static TreeNode* if_stmt();
static TreeNode* repeat_stmt();
static TreeNode* assign_stmt();
static TreeNode* read_stmt();
static TreeNode* write_stmt();
static TreeNode* while_stmt();
static TreeNode* expr();
static TreeNode* simple_exp();
static TreeNode* term();
static TreeNode* factor();

static void syntaxError(char* message)
{
    fprintf(listing, "\n>>> Error \n");
    fprintf(listing, "File \"%s\", line %d\n", filePath, lineno);
    fprintf(listing, "SyntaxError: %s", message);
    Error = true;
}

static void match(TokenType expected)
{
    if (currentToken == expected) {
        currentToken = getToken();
    }
    else {
        syntaxError("Unexpected token -> ");
        printToken(currentToken, tokenString);
        fprintf(listing, "\n");
    }
}

// Check if `currentToken` is end of a statement.
static bool isEnd()
{
    return (currentToken == ENDFILE) || (currentToken == ENDIF) ||
           (currentToken == ELSE) || (currentToken == UNTIL) ||
           (currentToken == ENDWHILE);
}

TreeNode* stmt_sequence()
{
    TreeNode* t = statement();
    TreeNode* p = t;
    while (!isEnd()) {
        TreeNode* q;
        q = statement();
        if (q != NULL) {
            if (t == NULL) {
                t = p = q;
            }
            else {
                p->sibling = q;
                p = q;
            }
        }
    }
    return t;
}

TreeNode* statement()
{
    TreeNode* t = NULL;
    switch (currentToken) {
    case IF:
        t = if_stmt();
        break;
    case REPEAT:
        t = repeat_stmt();
        break;
    case ID:
        t = assign_stmt();
        break;
    case READ:
        t = read_stmt();
        break;
    case WRITE:
        t = write_stmt();
        break;
    case WHILE:
        t = while_stmt();
        break;
    default:
        syntaxError("Unexpected token (statement) -> ");
        printToken(currentToken, tokenString);
        fprintf(listing, "\n");
        currentToken = getToken();
        break;
    } /* end case */
    return t;
}

TreeNode* if_stmt()
{
    TreeNode* stmt = newStmtNode(IfK);
    match(IF);
    if (stmt != NULL) {
        stmt->child[0] = expr();
    }
    match(THEN);
    if (stmt != NULL) {
        stmt->child[1] = stmt_sequence();
    }
    if (currentToken == ELSE) {
        match(ELSE);
        if (stmt != NULL) {
            stmt->child[2] = stmt_sequence();
        }
    }
    match(ENDIF);
    return stmt;
}

TreeNode* repeat_stmt()
{
    TreeNode* stmt = newStmtNode(RepeatK);
    if (stmt == NULL) {
        match(REPEAT);
        match(UNTIL);
        return NULL;
    }
    match(REPEAT);
    stmt->child[0] = stmt_sequence();
    match(UNTIL);
    stmt->child[1] = expr();
    match(SEMI);
    return stmt;
}

TreeNode* while_stmt()
{
    TreeNode* stmt = newStmtNode(WhileK);
    match(WHILE);
    if (stmt != NULL) {
        stmt->child[0] = expr();
        stmt->child[1] = stmt_sequence();
    }
    match(ENDWHILE);
    return stmt;
}

TreeNode* assign_stmt()
{
    TreeNode* stmt = newStmtNode(AssignK);
    if ((stmt != NULL) && (currentToken == ID)) {
        stmt->attr.name = copyString(tokenString);
    }
    match(ID);
    match(ASSIGN);
    if (stmt != NULL) {
        stmt->child[0] = expr();
    }
    match(SEMI);
    return stmt;
}

TreeNode* read_stmt()
{
    TreeNode* stmt = newStmtNode(ReadK);
    match(READ);
    if ((stmt != NULL) && (currentToken == ID)) {
        stmt->attr.name = copyString(tokenString);
    }
    match(ID);
    match(SEMI);
    return stmt;
}

TreeNode* write_stmt()
{
    TreeNode* stmt = newStmtNode(WriteK);
    match(WRITE);
    if (stmt != NULL) {
        stmt->child[0] = expr();
    }
    match(SEMI);
    return stmt;
}

TreeNode* expr()
{
    TreeNode* ex = simple_exp();
    if ((currentToken == LT) || (currentToken == EQ)) {
        TreeNode* op = newExpNode(OpK);
        if (op != NULL) {
            op->child[0] = ex;
            op->attr.op = currentToken;
            ex = op;
        }
        match(currentToken);
        if (ex != NULL) {
            ex->child[1] = simple_exp();
        }
    }
    return ex;
}

TreeNode* simple_exp()
{
    TreeNode* t = term();
    while ((currentToken == PLUS) || (currentToken == MINUS)) {
        TreeNode* p = newExpNode(OpK);
        if (p != NULL) {
            p->child[0] = t;
            p->attr.op = currentToken;
            t = p;
            match(currentToken);
            t->child[1] = term();
        }
    }
    return t;
}

TreeNode* term()
{
    TreeNode* f = factor();
    while ((currentToken == TIMES) || (currentToken == OVER)) {
        TreeNode* p = newExpNode(OpK);
        if (p != NULL) {
            p->child[0] = f;
            p->attr.op = currentToken;
            f = p;
            match(currentToken);
            p->child[1] = factor();
        }
    }
    return f;
}

TreeNode* factor()
{
    TreeNode* t = NULL;
    switch (currentToken) {
    case NUM:
        t = newExpNode(ConstK);
        if ((t != NULL) && (currentToken == NUM)) {
            t->attr.val = atoi(tokenString);
        }
        match(NUM);
        break;
    case ID:
        t = newExpNode(IdK);
        if ((t != NULL) && (currentToken == ID)) {
            t->attr.name = copyString(tokenString);
        }
        match(ID);
        break;
    case LPAREN:
        match(LPAREN);
        t = expr();
        match(RPAREN);
        break;
    default:
        syntaxError("unexpected token -> ");
        printToken(currentToken, tokenString);
        fprintf(listing, "\n");
        currentToken = getToken();
        break;
    }
    return t;
}

/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly
 * constructed syntax tree
 */
TreeNode* parse()
{
    TreeNode* t;
    currentToken = getToken();
    t = stmt_sequence();
    if (currentToken != ENDFILE) {
        syntaxError("Code ends before file\n");
        fprintf(listing, "\n");
    }
    return t;
}