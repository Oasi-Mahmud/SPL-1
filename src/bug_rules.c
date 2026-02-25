#include "bug_rules.h"
#include <stdio.h>
#include <string.h>


static int is_type_keyword_local(const char *s) {
    return strcmp(s,"int")==0 || strcmp(s,"double")==0 ||
           strcmp(s,"float")==0 || strcmp(s,"char")==0 ||
           strcmp(s,"boolean")==0 || strcmp(s,"long")==0 ||
           strcmp(s,"short")==0;
}

static int is_symbol(Token *t, int i, const char *sym) {
    return t[i].type == TOK_SYMBOL && strcmp(t[i].lexeme, sym) == 0;
}

static int is_keyword(Token *t, int i, const char *kw) {
    return t[i].type == TOK_KEYWORD && strcmp(t[i].lexeme, kw) == 0;
}

static int is_control_kw(const char *s) {
    return strcmp(s,"if")==0 || strcmp(s,"for")==0 || strcmp(s,"while")==0;
}
