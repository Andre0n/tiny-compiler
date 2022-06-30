/****************************************************/
/* File: scan.c                                     */
/* The scanner implementation for the TINY compiler */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "include/scan.h"

/* states in scanner DFA */
typedef enum { START, INASSIGN, INCOMMENT, INNUM, INID, DONE } StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN + 1]; // Problem with -Wstringop-overflow

/* BUFLEN = length of the input buffer for
   source code lines */
#define BUFLEN 256

static char lineBuf[BUFLEN];  /* holds the current line */
static int linepos = 0;       /* current position in LineBuf */
static int bufsize = 0;       /* current size of buffer string */
static bool EOF_flag = false; /* corrects ungetNextChar behavior on EOF */

/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
static int getNextChar(void)
{
    if (!(linepos < bufsize)) {
        lineno++;
        if (fgets(lineBuf, BUFLEN - 1, source)) {
            if (EchoSource) {
                fprintf(listing, "%4d: %s", lineno, lineBuf);
            }
            bufsize = strlen(lineBuf);
            linepos = 0;
            return lineBuf[linepos++];
        }
        else {
            EOF_flag = true;
            return EOF;
        }
    }
    else {
        return lineBuf[linepos++];
    }
}

/* ungetNextChar backtracks one character
   in lineBuf */
static void ungetNextChar(void)
{
    if (!EOF_flag) {
        linepos--;
    }
}

/* lookup table of reserved words */
static struct {
    char* str;
    TokenType tok;
} reservedWords[MAXRESERVED] = {
    {"if", IF},       {"then", THEN},         {"else", ELSE},
    {"endif", ENDIF}, {"endwhile", ENDWHILE}, {"repeat", REPEAT},
    {"until", UNTIL}, {"read", READ},         {"write", WRITE},
    {"while", WHILE}};

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
static TokenType reservedLookup(char* word)
{
    for (int i = 0; i < MAXRESERVED; i++) {
        if (!strcmp(word, reservedWords[i].str)) {
            return reservedWords[i].tok;
        }
    }
    return ID;
}

/****************************************/
/* the primary function of the scanner  */
/****************************************/
/* function getToken returns the
 * next token in source file
 */
TokenType getToken(void)
{ /* index for storing into tokenString */
    int tokenStringIndex = 0;
    /* holds current token to be returned */
    TokenType currentToken = ENDFILE;
    /* current state - always begins at START */
    StateType state = START;
    /* Solving `[-Wstringop-overflow]` write a byte in size 0
    /* holds the current `tokenString` temporarily.
    */
    char tokenLexeme[MAXTOKENLEN + 1] = "";

    while (state != DONE) {
        int c = getNextChar();
        bool save = true;
        switch (state) {
        case START:
            if (isdigit(c)) {
                state = INNUM;
            }
            else if (isalpha(c)) {
                state = INID;
            }
            else if (c == ':') {
                state = INASSIGN;
            }
            else if (isspace(c)) {
                save = false;
            }
            else if (c == '{') {
                save = false;
                state = INCOMMENT;
            }
            else {
                state = DONE;
                switch (c) {
                case EOF:
                    save = false;
                    currentToken = ENDFILE;
                    break;
                case '=':
                    currentToken = EQ;
                    break;
                case '<':
                    currentToken = LT;
                    break;
                case '+':
                    currentToken = PLUS;
                    break;
                case '-':
                    currentToken = MINUS;
                    break;
                case '*':
                    currentToken = TIMES;
                    break;
                case '/':
                    currentToken = OVER;
                    break;
                case '(':
                    currentToken = LPAREN;
                    break;
                case ')':
                    currentToken = RPAREN;
                    break;
                case ';':
                    currentToken = SEMI;
                    break;
                default:
                    currentToken = ERROR;
                    break;
                }
            }
            break;
        case INCOMMENT:
            save = false;
            if (c == EOF) {
                state = DONE;
                currentToken = ENDFILE;
            }
            else if (c == '}') {
                state = START;
            }
            break;
        case INASSIGN:
            state = DONE;
            if (c == '=') {
                currentToken = ASSIGN;
            }
            else { /* backup in the input */
                ungetNextChar();
                save = false;
                currentToken = DDOT;
            }
            break;
        case INNUM:
            if (!isdigit(c)) { /* backup in the input */
                ungetNextChar();
                save = false;
                state = DONE;
                currentToken = NUM;
            }
            break;
        case INID:
            if (!(isalnum(c)) && (c != '_')) { /* backup in the input */
                ungetNextChar();
                save = false;
                state = DONE;
                currentToken = ID;
            }
            break;
        case DONE:
        default: /* should never happen */
            fprintf(listing, "Scanner Bug: state= %d\n", state);
            state = DONE;
            currentToken = ERROR;
            break;
        }
        if ((save) && (tokenStringIndex <= MAXTOKENLEN)) {
            char tmp[] = {(char)c, '\0'};
            strcat(tokenLexeme, tmp);
            tokenStringIndex++;
        }
        if (state == DONE) {
            strcpy(tokenString, tokenLexeme);
            if (currentToken == ID) {
                currentToken = reservedLookup(tokenString);
            }
            memset(tokenLexeme, 0, MAXTOKENLEN);
        }
    }
    if (TraceScan) {
        fprintf(listing, "\t%d: ", lineno);
        printToken(currentToken, tokenString);
    }
    return currentToken;
} /* end getToken */