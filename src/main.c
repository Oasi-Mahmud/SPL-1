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
