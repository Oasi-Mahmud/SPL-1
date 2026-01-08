#include <stdio.h>
#include <string.h>
#include "token.h"

static int is_type_keyword(const char *s) {
    return strcmp(s,"int")==0 || strcmp(s,"double")==0 ||
           strcmp(s,"float")==0 || strcmp(s,"char")==0 ||
           strcmp(s,"boolean")==0 || strcmp(s,"long")==0 ||
           strcmp(s,"short")==0;
}

void analyze(Token *t, int n) {

    int classCount = 0, methodCount = 0;
    int forCount = 0, whileCount = 0, doCount = 0;
    int globalVar = 0, localVar = 0;
    int LOC = 0;

    int braceDepth = 0;
    int insideMethod = 0;

    printf("\nDetected Classes:\n");

      for (int i = 0; i < n - 1; i++) {
        if (t[i].type == TOK_KEYWORD &&
            strcmp(t[i].lexeme, "class") == 0 &&
            t[i+1].type == TOK_IDENTIFIER) {

            printf("  Class: %s (line %d)\n",
                   t[i+1].lexeme, t[i].line);
            classCount++;
        }
    }

    printf("\nDetected Methods:\n");

    for (int i = 0; i < n - 2; i++) {
        if ((t[i].type == TOK_KEYWORD && is_type_keyword(t[i].lexeme)) &&
            t[i+1].type == TOK_IDENTIFIER &&
            strcmp(t[i+2].lexeme, "(") == 0) {

            printf("  Method: %s (line %d)\n",
                   t[i+1].lexeme, t[i].line);
            methodCount++;
        }

        if (t[i].type == TOK_KEYWORD &&
            strcmp(t[i].lexeme,"void")==0 &&
            t[i+1].type == TOK_IDENTIFIER &&
            strcmp(t[i+2].lexeme,"(")==0) {

            printf("  Method: %s (line %d)\n",
                   t[i+1].lexeme, t[i].line);
            methodCount++;
        }
    }

    printf("\nDetected Loops:\n");

    for (int i = 0; i < n; i++) {
        if (t[i].type == TOK_KEYWORD) {
            if (strcmp(t[i].lexeme,"for")==0) {
                printf("  for-loop (line %d)\n", t[i].line);
                forCount++;
            }
            if (strcmp(t[i].lexeme,"while")==0) {
                printf("  while-loop (line %d)\n", t[i].line);
                whileCount++;
            }
            if (strcmp(t[i].lexeme,"do")==0) {
                printf("  do-while-loop (line %d)\n", t[i].line);
                doCount++;
            }
        }
    }

