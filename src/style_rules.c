#include "style_rules.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>


static int is_type_keyword_local(const char *s) {
    return strcmp(s,"int")==0 || strcmp(s,"double")==0 ||
           strcmp(s,"float")==0 || strcmp(s,"char")==0 ||
           strcmp(s,"boolean")==0 || strcmp(s,"long")==0 ||
           strcmp(s,"short")==0;
}

static int has_underscore(const char *s) {
    for (int i = 0; s[i]; i++){
        if (s[i] == '_')
            return 1;
    }
    return 0;
}

static int is_pascal_case(const char *s) {
    if (!s || !s[0])
        return 0;
    if (!isupper((unsigned char)s[0]))
        return 0;
    if (has_underscore(s))
        return 0;
    return 1;
}

static int is_camel_case(const char *s) {
    if (!s || !s[0]) 
        return 0;
    if (!islower((unsigned char)s[0]))
        return 0;
    if (has_underscore(s)) 
        return 0;
    return 1;
}

static int looks_like_constant(const char *s) {
    int hasAlpha = 0;
    for (int i = 0; s[i]; i++) {
        char c = s[i];
        if (isalpha((unsigned char)c)) {
            hasAlpha = 1;
            if (!isupper((unsigned char)c))
                return 0;
        } 
        else if (c == '_' || isdigit((unsigned char)c)) {
            
        } else {
            return 0;
        }
    }
    return hasAlpha;
}
static int is_control_keyword(const char *s) {
    return strcmp(s,"if")==0 || strcmp(s,"for")==0 ||
           strcmp(s,"while")==0 || strcmp(s,"do")==0;
}

static int skip_parens(Token *t, int n, int i) {
    if (i >= n) 
        return i;
    if (!(t[i].type == TOK_SYMBOL && strcmp(t[i].lexeme, "(") == 0)) 
        return i;

    int depth = 0;
    for (; i < n; i++) {
        if (t[i].type == TOK_SYMBOL && strcmp(t[i].lexeme, "(") == 0) depth++;
        else if (t[i].type == TOK_SYMBOL && strcmp(t[i].lexeme, ")") == 0) {
            depth--;
            if (depth == 0)
                return i + 1;
        }
    }
    return n - 1;
}


void run_style_checks(Token *t, int n) {
    if (!t || n <= 0)
        return;

    printf("\nStyle Checks (Rule-based):\n");
    int warningCount = 0;

    for (int i = 0; i < n - 1; i++) {
        if (t[i].type == TOK_KEYWORD &&
            strcmp(t[i].lexeme, "class") == 0 &&
            t[i+1].type == TOK_IDENTIFIER) {

            const char *cls = t[i+1].lexeme;
            if (!is_pascal_case(cls)) {
                printf("  [STYLE] Class name should be PascalCase: '%s' (line %d)\n",
                       cls, t[i].line);
                warningCount++;
            }
        }
    }

    for (int i = 0; i < n - 2; i++) {
        int isReturnType =
            (t[i].type == TOK_KEYWORD && is_type_keyword_local(t[i].lexeme)) ||
            (t[i].type == TOK_KEYWORD && strcmp(t[i].lexeme, "void") == 0);

        if (isReturnType &&
            t[i+1].type == TOK_IDENTIFIER &&
            strcmp(t[i+2].lexeme, "(") == 0) {

            const char *m = t[i+1].lexeme;

            if (isupper((unsigned char)m[0])) {
                continue;
            }

            if (!is_camel_case(m)) {
                printf("  [STYLE] Method name should be camelCase: '%s' (line %d)\n",
                       m, t[i].line);
                warningCount++;
            }
        }
    }
for (int i = 0; i < n - 1; i++) {
        if (t[i].type == TOK_KEYWORD && is_type_keyword_local(t[i].lexeme) &&
            t[i+1].type == TOK_IDENTIFIER) {

            const char *v = t[i+1].lexeme;

            if (looks_like_constant(v))
                continue;

            if (!is_camel_case(v)) {
                printf("  [STYLE] Variable name should be camelCase: '%s' (line %d)\n",
                       v, t[i].line);
                warningCount++;
            }
        }
    }

    for (int i = 0; i < n - 1; i++) {
        if (t[i].type == TOK_KEYWORD && is_control_keyword(t[i].lexeme)) {

            int j = i + 1;

            if (strcmp(t[i].lexeme, "do") != 0) {
                while (j < n && !(t[j].type == TOK_SYMBOL && strcmp(t[j].lexeme, "(") == 0)) j++;
                if (j >= n) 
                    continue;

                j = skip_parens(t, n, j); 
                if (j >= n)
                    continue;
            }

            if (!(t[j].type == TOK_SYMBOL && strcmp(t[j].lexeme, "{") == 0)) {
                printf("  [STYLE] Use braces with '%s' statement (line %d)\n",
                       t[i].lexeme, t[i].line);
                warningCount++;
            }
        }
    }

 for (int i = 1; i < n; i++) {
        if (t[i].type == TOK_KEYWORD && strcmp(t[i].lexeme, "else") == 0) {
         
            if (!(t[i-1].type == TOK_SYMBOL && strcmp(t[i-1].lexeme, "}") == 0)) {
                printf("  [STYLE] 'else' should be aligned with previous block (prefer '} else {') (line %d)\n",
                       t[i].line);
                warningCount++;
            }
        }
    }

 for (int i = 0; i < n; i++) {
        if (t[i].type == TOK_KEYWORD && strcmp(t[i].lexeme, "case") == 0) {

            int j = i;
            int hasBreak = 0;

            for (; j < n; j++) {
                if (j != i && t[j].type == TOK_KEYWORD &&
                    (strcmp(t[j].lexeme, "case") == 0 || strcmp(t[j].lexeme, "default") == 0)) {
                    break;
                }

                if (t[j].type == TOK_KEYWORD && strcmp(t[j].lexeme, "break") == 0) {
                    hasBreak = 1;
                }

                if (t[j].type == TOK_SYMBOL && strcmp(t[j].lexeme, "}") == 0) {
                    break;
                }
            }

       if (!hasBreak) {
                printf("  [STYLE] case may need 'break' to avoid fall-through (line %d)\n",
                       t[i].line);
                warningCount++;
            }
        }
    }
