#include <stdio.h>
#include <string.h>
#include "token.h"

static int is_type_keyword(const char *s) {
    return strcmp(s,"int")==0 || strcmp(s,"double")==0 ||
           strcmp(s,"float")==0 || strcmp(s,"char")==0 ||
           strcmp(s,"boolean")==0 || strcmp(s,"long")==0 ||
           strcmp(s,"short")==0;
}

const char* infer_method_purpose(const char *name) {

    if (strncmp(name,"get",3)==0)
        return "Returns the value of a variable";

    if (strncmp(name,"set",3)==0)
        return "Sets or updates the value of a variable";

    if (strncmp(name,"is",2)==0)
        return "Checks a condition and returns boolean result";

    if (strstr(name,"area") || strstr(name,"Area"))
        return "Calculates area of a geometric shape";

     if (strstr(name,"distance") || strstr(name,"Distance"))
        return "Calculates distance between objects or points";

    if (strcmp(name,"toString")==0)
        return "Returns string representation of the object";


    return "Performs a programmer-defined operation";
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

     printf("\nDetected Variables:\n");

    for (int i =0; i <n - 1; i++) {

        if (t[i].type ==TOK_SYMBOL) {
            if (strcmp(t[i].lexeme,"{")==0)
                 braceDepth++;
            if (strcmp(t[i].lexeme,"}")==0) {
                braceDepth--;
                if (braceDepth < 2) insideMethod = 0;
            }
        }

        if (t[i].type== TOK_KEYWORD && is_type_keyword(t[i].lexeme) &&
            t[i+1].type ==TOK_IDENTIFIER) {

            if (braceDepth >= 2) {
                printf(" Local Variable: %s (line %d)\n",
                       t[i+1].lexeme, t[i].line);
                      localVar++;
            } else {
                printf("  Global Variable: %s (line %d)\n",
                       t[i+1].lexeme, t[i].line);
                globalVar++;
            }
        }
    }

    int lastLine= -1;                 
    for (int i = 0; i <n; i++) {
        if (t[i].line !=lastLine &&
            t[i].type !=TOK_EOF) {
            LOC++;
            lastLine = t[i].line;
        }
    }

 printf("\nSummary  :\n");
    printf("  Total Classes        : %d \n", classCount);
    printf(" Total Methods        : %d \n", methodCount);
    printf("  for-loops            : %d\n", forCount);
    printf(" while-loops          : %d\n", whileCount);
    printf("  do-while-loops      : %d\n", doCount);
    printf("  Global Variables    : %d\n", globalVar);
    printf("  Local Variables    : %d\n", localVar);
    printf("  Lines of Code (LOC)   : %d\n", LOC);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <java_file>\n", argv[0]);
        return 1;
    }

    int count = 0;
    Token *tokens = lex_file(argv[1], &count);
    if (!tokens) {
        fprintf(stderr, "Tokenization failed\n");
        return 1;
    }

    analyze(tokens, count);
    free_tokens(tokens, count);
    return 0;
}

.
