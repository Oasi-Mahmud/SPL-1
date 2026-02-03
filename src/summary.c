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

