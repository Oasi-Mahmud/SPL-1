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

static void init_array(TokenArray *arr) {
    arr->data = NULL;
    arr->size = 0;
    arr->capacity = 0;
}
static void push_token(TokenArray *arr, Token t) {
    if (arr->size == arr->capacity) {
        int newCap = (arr->capacity == 0) ? 64 : arr->capacity * 2;
        Token *tmp = realloc(arr->data, newCap * sizeof(Token));
        if (!tmp) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
        arr->data = tmp;
        arr->capacity=newCap;
    }
    arr->data[arr->size++] = t;
}

static char *copy_range(const char *src, int len) {
    char *s = malloc(len + 1);
    if (!s) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    strncpy(s, src, len);
    s[len] = '\0';
    return s;
}

static const char *keywords[] = {
    "class","public","private","protected","static",
    "void","int","double","float","boolean","char",
    "long","short","return","if","else","for","while",
    "do","switch","case","break","continue","new",
    "try","catch","finally","throws","extends","implements"
};

static int is_keyword(const char *s) {
    int n = sizeof(keywords) / sizeof(keywords[0]);
    for (int i = 0; i < n; i++) {
        if (strcmp(s, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

Token *lex_file(const char *filename, int *out_count) {
    FILE *fp = fopen(filename, "r");
    
    if (!fp) {
        fprintf(stderr, "Cannot open file %s\n", filename);
        return NULL;
    }

    TokenArray tokens;
    init_array(&tokens);

    int c;
    int line = 1, col = 0;

    while ((c = fgetc(fp)) != EOF) {
        col++;
        if (c == ' ' || c == '\t' || c == '\r')
            continue;

            if (c == '\n') {
            line++;
            col = 0;
            continue;
        }
        
         if (c == '/') {
            int next = fgetc(fp);
         if (next == '/') {
                while ((c = fgetc(fp)) != EOF && c != '\n');
                line++;
                col = 0;
                continue;
            }

            if (next == '*') {
                int prev = 0;
                while ((c = fgetc(fp)) != EOF) {
                    if (c == '\n') {
                        line++;
                        col = 0;
                    }
                    
                    if (prev == '*' && c == '/')
                        break;
                    prev = c;
                }
                
                continue;
            }

            ungetc(next, fp);
        }
        
          if (isalpha(c) || c == '_' || c == '$') {
            char buf[256];
            int len = 0;

            buf[len++] = c;

            int p;
            while ((p = fgetc(fp)) != EOF &&
                   (isalnum(p) || p == '_' || p == '$')) {
                if (len < 255)
                    buf[len++] = p;
                col++;
            }
              
            buf[len] = '\0';
            if (p != EOF) ungetc(p, fp);

            Token t;
            t.type = is_keyword(buf) ? TOK_KEYWORD : TOK_IDENTIFIER;
            t.lexeme = strdup(buf);
            t.line = line;
            t.column = col - len + 1;

            push_token(&tokens, t);
            continue;
        }
        
          if (isdigit(c)) {
            char buf[128];
            int len = 0;

            buf[len++] = c;
            int p;
            while ((p = fgetc(fp)) != EOF && isdigit(p)) {
                if (len < 127)
                    buf[len++] = p;
                col++;
            }
              
            buf[len] = '\0';
            if (p != EOF) ungetc(p, fp);

            Token t;
            t.type = TOK_NUMBER;
            t.lexeme = strdup(buf);
            t.line = line;
            t.column = col - len + 1;

            push_token(&tokens, t);
            continue;
        }

       
        if (c == '"') {
            char buf[512];
            int len =0;
            int p;

            while ((p = fgetc(fp)) != EOF && p != '"') {
                if (p == '\n') {
                    line++;
                    col =0;
                }
                if (len < 511)
                    buf[len++] =p;
            }
            buf[len] = '\0';

            Token t;
            t.type =TOK_STRING;
            t.lexeme= strdup(buf);
            t.line= line;
            t.column =col;

            push_token(&tokens, t);
            continue;
        }     
        {
            char sym[2] = {c, '\0'};
            Token t;
            t.type =TOK_SYMBOL;
            t.lexeme= strdup(sym);
            t.line =line;
            t.column=col;
            push_token(&tokens, t);
        }
    }
            Token eof;
            eof.type =TOK_EOF;
            eof.lexeme= strdup("EOF");
            eof.line =line;
            eof.column =col;
            push_token(&tokens, eof);
        
            fclose(fp);
            *out_count= tokens.size;
            return tokens.data;
        }
        void free_tokens(Token *tokens, int count) {
            if (!tokens) return;
            for (int i= 0;i <count;i++)
                free(tokens[i].lexeme);
            free(tokens);
        }
