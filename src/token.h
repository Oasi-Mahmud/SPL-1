#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    TOK_IDENTIFIER,
    TOK_KEYWORD,
    TOK_NUMBER,
    TOK_STRING,
    TOK_SYMBOL,
    TOK_EOF
} JTokenType;

typedef struct {
    JTokenType type;
    char *lexeme;
    int line;
    int column;
} Token;

Token *lex_file(const char *filename, int *out_count);
void free_tokens(Token *tokens, int count);

#endif
