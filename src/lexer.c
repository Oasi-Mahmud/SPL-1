#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "token.h"

typedef struct {
    Token *data;
    int size;
    int capacity;
} TokenArray;

static void init_token_array(TokenArray *arr) {
    arr->data = NULL;
    arr->size = 0;
    arr->capacity = 0;
}

static void push_token(TokenArray *arr, Token t) {
    if (arr->size == arr->capacity) {
        int new_cap = (arr->capacity == 0) ? 64 : arr->capacity * 2;
        Token *new_data = (Token *)realloc(arr->data, new_cap * sizeof(Token));
        if (!new_data) {
            fprintf(stderr, "Memory allocation failed for tokens\n");
            exit(1);
        }
        arr->data = new_data;
        arr->capacity = new_cap;
    }
    arr->data[arr->size++] = t;
}

static char *strdup_range(const char *start, int len) {
    char *s = (char *)malloc(len + 1);
    if (!s) {
        fprintf(stderr, "Memory allocation failed for lexeme\n");
        exit(1);
    }
    memcpy(s, start, len);
    s[len] = '\0';
    return s;
}

static int is_keyword(const char *s) {
    const char *keywords[] = {
        "class", "public", "private", "protected",
        "static", "void", "int", "double", "float",
        "boolean", "if", "else", "for", "while", "do",
        "return", "new", "extends", "implements"
    };
    int n = sizeof(keywords) / sizeof(keywords[0]);
    for (int i = 0; i < n; ++i) {
        if (strcmp(s, keywords[i]) == 0) return 1;
    }
    return 0;
}

Token *lex_file(const char *filename, int *out_count) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Could not open file: %s\n", filename);
        *out_count = 0;
        return NULL;
    }

    TokenArray tokens;
    init_token_array(&tokens);

    int c;
    int line = 1;
    int col = 0;

    while ((c = fgetc(f)) != EOF) {
        col++;

        // white space........
        if (c == ' ' || c == '\t' || c == '\r') {
            continue;
        }
        if (c == '\n') {
            line++;
            col = 0;
            continue;
        }
        // comments.....
        if (c == '/') {
            int next = fgetc(f);
            if (next == '/') {
                // single line comment........
                while ((c = fgetc(f)) != EOF && c != '\n')
                    ;
                if (c == '\n') {
                    line++;
                    col = 0;
                }
                continue;
            } else if (next == '*') {
               // blocking comments........
                int prev = 0;
                while ((c = fgetc(f)) != EOF) {
                    if (c == '\n') {
                        line++;
                        col = 0;
                    }
                    if (prev == '*' && c == '/') {
                        break;
                    }
                    prev = c;
                }
                continue;
            } else {
                // just a single '/'.......
                ungetc(next, f);     
                Token t;
                t.type = TOK_SYMBOL;
                t.lexeme = strdup_range("/", 1);
                t.line = line;
                t.column = col;
                push_token(&tokens, t);
                continue;
            }
        }
