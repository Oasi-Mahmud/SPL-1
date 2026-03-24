#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "token.h"
#include "summary.h"
#include "style_rules.h"
#include "bug_rules.h"


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

    char methodNames[100][64];
    char classNames[100][64];
    int methodLines[100];
    int classLines[100];
    
    printf("\nDetected Classes:\n");

      for (int i = 0; i < n - 1; i++) {
        if (t[i].type == TOK_KEYWORD &&
            strcmp(t[i].lexeme, "class") == 0 &&
            t[i+1].type == TOK_IDENTIFIER) {

            printf("  Class: %s (line %d)\n",
                   t[i+1].lexeme, t[i].line);
             if (classCount < 100) {
                strcpy(classNames[classCount], t[i+1].lexeme);
                classLines[classCount] = t[i].line;
            }
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
             if (methodCount < 100) {
                strcpy(methodNames[methodCount], t[i+1].lexeme);
                methodLines[methodCount] = t[i].line;
            }
            methodCount++;
        }

        if (t[i].type == TOK_KEYWORD &&
            strcmp(t[i].lexeme,"void")==0 &&
            t[i+1].type == TOK_IDENTIFIER &&
            strcmp(t[i+2].lexeme,"(")==0) {

            printf("  Method: %s (line %d)\n",
                   t[i+1].lexeme, t[i].line);
            if (methodCount < 100) {
                strcpy(methodNames[methodCount], t[i+1].lexeme);
                methodLines[methodCount] = t[i].line;
            }
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

    int variableCount = 0;

    for (int i = 0; i < n - 1; i++) {

    if (t[i].type == TOK_KEYWORD && is_type_keyword(t[i].lexeme) &&
        t[i+1].type == TOK_IDENTIFIER) {

        printf("  Variable: %s (line %d)\n",
               t[i+1].lexeme, t[i].line);

        variableCount++;
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

    run_style_checks(t, n);
    run_bug_checks(t, n);

    char choice;
    printf("\nDo you want to see summary? (y/n): ");
    scanf(" %c", &choice);

    if (choice == 'y' || choice == 'Y') {

        printf("\n1. Method Summary\n");
        printf("2. Class Summary\n");
        printf("Select option: ");

        int opt;
        scanf("%d", &opt);

        if (opt == 1 && methodCount > 0) {

            printf("\nAvailable Methods:\n");

            int printableMethods = (methodCount < 100) ? methodCount : 100;

            for (int i = 0; i < printableMethods; i++)
                printf("  %d. %s\n", i+1, methodNames[i]);

            int sel;
            printf("Enter method number: ");
            scanf("%d", &sel);

            if (sel >= 1 && sel <= printableMethods) {
                char buffer[256];
                method_name_summary(methodNames[sel-1], buffer, sizeof(buffer));

                printf("\nMethod Summary:\n");
                printf("  Name    : %s\n", methodNames[sel-1]);
                printf("  Line    : %d\n", methodLines[sel-1]);
                printf("  Purpose : %s\n", buffer);
            }
}
 else if (opt == 2 && classCount > 0) {

            printf("\nAvailable Classes:\n");

            int printableClasses = (classCount < 100) ? classCount : 100;

            for (int i = 0; i < printableClasses; i++)
                printf("  %d. %s\n", i+1, classNames[i]);

            int sel;
            printf("Enter class number: ");
            scanf("%d", &sel);

            if (sel >= 1 && sel <= printableClasses) {
                char buffer[256];
                class_name_summary(classNames[sel-1], buffer, sizeof(buffer));

                printf("\nClass Summary:\n");
                printf("  Name    : %s\n", classNames[sel-1]);
                printf("  Line    : %d\n", classLines[sel-1]);
                printf("  Purpose : %s\n", buffer);
            }
        }
    }
}


int main() {
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
