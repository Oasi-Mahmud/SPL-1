#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

