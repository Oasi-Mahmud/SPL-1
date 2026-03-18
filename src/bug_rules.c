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
static int skip_parens(Token *t, int n, int i) {
    if (i >= n)
        return i;
    if (!is_symbol(t, i, "("))
        return i;

    int depth = 0;
    for (; i < n; i++) {
        if (is_symbol(t, i, "(")) 
            depth++;
        else if (is_symbol(t, i, ")")) {
            depth--;
            if (depth == 0) 
                return i + 1;
        }
    }
    return n;
}

static int find_next_lparen(Token *t, int n, int i) {
    int j = i;
    while (j < n && !(t[j].type == TOK_SYMBOL && strcmp(t[j].lexeme, "(") == 0))
        j++;
    return j;
}

void run_bug_checks(Token *t, int n) {
    if (!t || n <= 0)
        return;

    printf("\nBug Pattern Checks (Rule-based):\n");
    int warn = 0;

    for (int i = 0; i < n - 2; i++) {
        if (t[i].type == TOK_KEYWORD && is_control_kw(t[i].lexeme)) {

            int j = find_next_lparen(t, n, i + 1);
            if (j >= n)
               continue;

            j = skip_parens(t, n, j);
            if (j >= n)
               continue;

            if (j < n && is_symbol(t, j, ";")) {
                printf("  [BUG-1] Suspicious ';' after %s-condition (empty body) (line %d)\n",
                       t[i].lexeme, t[i].line);
                warn++;
                continue;
            }

            if (j + 1 < n && is_symbol(t, j, "{") && is_symbol(t, j + 1, "}")) {
                printf("  [BUG-1] Empty block after %s-condition (line %d)\n",
                       t[i].lexeme, t[i].line);
                warn++;
                continue;
            }
        }
    }

    for (int i = 0; i < n - 2; i++) {
        if (is_keyword(t, i, "if")) {
            int j = find_next_lparen(t, n, i + 1);
            if (j >= n) 
                continue;

            int end = skip_parens(t, n, j);
            if (end > n) 
                end = n;

            for (int k = j; k < end - 1; k++) {
                if (is_symbol(t, k, "=")) {
                    if (k + 1 < n && is_symbol(t, k + 1, "="))
                        continue;
                   
                    if (k - 1 >= 0 && (is_symbol(t, k - 1, "!") || is_symbol(t, k - 1, "<") || is_symbol(t, k - 1, ">")))
                        continue;

                    printf("  [BUG-2] Possible assignment inside if-condition (line %d)\n",
                           t[i].line);
                    warn++;
                    break;
                }
            }
        }
    }
for (int i = 0; i < n - 1; i++) {
        if (is_symbol(t, i, "/")) {
            if (t[i + 1].type == TOK_NUMBER && strcmp(t[i + 1].lexeme, "0") == 0) {
                printf("  [BUG-3] Possible division by zero (line %d)\n", t[i].line);
                warn++;
            }
            if (i + 3 < n &&
                is_symbol(t, i + 1, "(") &&
                t[i + 2].type == TOK_NUMBER && strcmp(t[i + 2].lexeme, "0") == 0 &&
                is_symbol(t, i + 3, ")")) {
                printf("  [BUG-3] Possible division by zero (line %d)\n", t[i].line);
                warn++;
            }
        }
    }

    char localNames[200][64];
    int localLine[200];
    int localCount = 0;

    int braceDepth = 0;

    for (int i = 0; i < n - 1; i++) {
        if (t[i].type == TOK_SYMBOL) {
            if (is_symbol(t, i, "{"))
                braceDepth++;
            else if (is_symbol(t, i, "}"))
                braceDepth--;
        }

        if (braceDepth >= 2 &&
            t[i].type == TOK_KEYWORD && is_type_keyword_local(t[i].lexeme) &&
            t[i + 1].type == TOK_IDENTIFIER) {

            if (localCount < 200) {
                strncpy(localNames[localCount], t[i + 1].lexeme, 63);
                localNames[localCount][63] = '\0';
                localLine[localCount] = t[i].line;
                localCount++;
            }
        }
    }

 for (int k = 0; k < localCount; k++) {
        int occ = 0;
        for (int i = 0; i < n; i++) {
            if (t[i].type == TOK_IDENTIFIER && strcmp(t[i].lexeme, localNames[k]) == 0)
                occ++;
        }
        if (occ <= 1) {
            printf("  [BUG-4] Unused local variable '%s' (declared line %d)\n",
                   localNames[k], localLine[k]);
            warn++;
        }
    }

  for (int i = 0; i < n - 4; i++) {
        if (is_keyword(t, i, "catch")) {
            int j = find_next_lparen(t, n, i + 1);
            if (j >= n)
             continue;

            j = skip_parens(t, n, j);
            if (j + 1 < n && is_symbol(t, j, "{") && is_symbol(t, j + 1, "}")) {
                printf("  [BUG-5] Empty catch block (line %d)\n", t[i].line);
                warn++;
            }
        }
    }

 for (int i = 0; i < n - 1; i++) {
        if (is_keyword(t, i, "while")) {
            int j = find_next_lparen(t, n, i + 1);
            if (j >= n) 
            continue;

            int end = skip_parens(t, n, j);
            if (end > n) 
            end = n;

            for (int k = j; k < end; k++) {
                if (t[k].type == TOK_IDENTIFIER && strcmp(t[k].lexeme, "true") == 0) {
                    printf("  [BUG-6] Possible infinite loop: while(true) (line %d)\n", t[i].line);
                    warn++;
                    break;
                }
            }
        }
 if (is_keyword(t, i, "for")) {
            int j = find_next_lparen(t, n, i + 1);
            if (j >= n) 
              continue;

            if (j + 2 < n && is_symbol(t, j, "(") && is_symbol(t, j + 1, ";") && is_symbol(t, j + 2, ";")) {
                printf("  [BUG-6] Possible infinite loop: for(;;) (line %d)\n", t[i].line);
                warn++;
            }
        }
    }
 if (is_keyword(t, i, "for")) {
            int j = find_next_lparen(t, n, i + 1);
            if (j >= n) 
              continue;

            if (j + 2 < n && is_symbol(t, j, "(") && is_symbol(t, j + 1, ";") && is_symbol(t, j + 2, ";")) {
                printf("  [BUG-6] Possible infinite loop: for(;;) (line %d)\n", t[i].line);
                warn++;
            }
        }
    }
