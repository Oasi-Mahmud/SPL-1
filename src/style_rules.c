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
    for (int i = 0; s[i]; i++) if (s[i] == '_') return 1;
    return 0;
}

static int is_pascal_case(const char *s) {
    if (!s || !s[0]) return 0;
    if (!isupper((unsigned char)s[0])) return 0;
    if (has_underscore(s)) return 0;
    return 1;
}

static int is_camel_case(const char *s) {
    if (!s || !s[0]) return 0;
    if (!islower((unsigned char)s[0])) return 0;
    if (has_underscore(s)) return 0;
    return 1;
}

static int looks_like_constant(const char *s) {
    int hasAlpha = 0;
    for (int i = 0; s[i]; i++) {
        char c = s[i];
        if (isalpha((unsigned char)c)) {
            hasAlpha = 1;
            if (!isupper((unsigned char)c)) return 0;
        } else if (c == '_' || isdigit((unsigned char)c)) {
            
        } else {
            return 0;
        }
    }
    return hasAlpha;
}
