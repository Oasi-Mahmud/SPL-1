#include "summary.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

static void to_lower_copy(const char *in, char *out, size_t outsz) {
    size_t j = 0;
    for (size_t i = 0; in[i] && j + 1 < outsz; i++) {
        out[j++] = (char)tolower((unsigned char)in[i]);
    }
    out[j] = '\0';
}

static void normalize_name(const char *in, char *out, size_t outsz) {
    char tmp[256];
    to_lower_copy(in, tmp, sizeof(tmp));

    size_t j = 0;
    for (size_t i = 0; tmp[i] && j + 1 < outsz; i++) {
        char c = tmp[i];
        if (c == '_' || c == '-' || c == ' ') continue;
        out[j++] = c;
    }
    out[j] = '\0';

    while (j > 0 && isdigit((unsigned char)out[j - 1])) {
        out[j - 1] = '\0';
        j--;
    }
}

static int starts_with(const char *s, const char *p) {
    return strncmp(s, p, strlen(p)) == 0;
}

static void extract_object_phrase(const char *original, int prefixLen, char *out, size_t outsz) {
    const char *p = original + prefixLen;
    while (*p == '_' || *p == '-' || *p == ' ') p++;

    size_t j = 0;
    int prevWasSpace = 1;

    for (size_t i = 0; p[i] && j + 1 < outsz; i++) {
        char c = p[i];

        if (c == '_' || c == '-' ) {
            if (!prevWasSpace && j + 1 < outsz) out[j++] = ' ';
            prevWasSpace = 1;
            continue;
        }

        if (isupper((unsigned char)c)) {
            if (!prevWasSpace && j + 1 < outsz) out[j++] = ' ';
            out[j++] = (char)tolower((unsigned char)c);
            prevWasSpace = 0;
        } else {
         
            if (c == ' ') {
                if (!prevWasSpace && j + 1 < outsz) out[j++] = ' ';
                prevWasSpace = 1;
            } else {
                out[j++] = (char)tolower((unsigned char)c);
                prevWasSpace = 0;
            }
        }
    }

    while (j > 0 && out[j - 1] == ' ') j--;
    out[j] = '\0';
}

static void write_out(char *out, size_t outsz, const char *fmt, const char *obj) {
    if (obj && obj[0]) snprintf(out, outsz, fmt, obj);
    else snprintf(out, outsz, "%s", "Performs a programmer-defined operation");
}

void method_name_summary(const char *methodName, char *out, size_t outSize) {
    if (!methodName || !out || outSize == 0) return;

    char norm[256];
    normalize_name(methodName, norm, sizeof(norm));

    if (strcmp(norm, "tostring") == 0) {
        snprintf(out, outSize, "Returns string representation of the object");
        return;
    }

    if (starts_with(norm, "is")) {
        char obj[256];
        extract_object_phrase(methodName, 2, obj, sizeof(obj));
        if (obj[0]) snprintf(out, outSize, "Checks whether %s", obj);
        else snprintf(out, outSize, "Checks a condition and returns boolean result");
        return;
    }
    if (starts_with(norm, "has")) {
        char obj[256];
        extract_object_phrase(methodName, 3, obj, sizeof(obj));
        if (obj[0]) snprintf(out, outSize, "Checks whether it has %s", obj);
        else snprintf(out, outSize, "Checks existence and returns boolean result");
        return;
    }
