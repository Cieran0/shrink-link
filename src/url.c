#include "stdlib.h"
#include "unistd.h"
#include "string.h"

const char useable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

size_t useable_lookup[256];

void init_url_char_lookup_table() {
    for (size_t i = 0; i < 256; i++) {
        useable_lookup[i] = -1;
    }
    for (size_t i = 0; i < 64; i++) {
        useable_lookup[(unsigned char)useable[i]] = i;
    }
}


char* number_to_url(size_t num) {
    char* base64_str = malloc(12);
    if (!base64_str) return NULL;

    if (num == 0) {
        base64_str[0] = useable[0];
        base64_str[1] = '\0';
        return base64_str;
    }

    int i = 0;
    while (num > 0) {
        base64_str[i++] = useable[num % 64];
        num /= 64;
    }

    base64_str[i] = '\0';

    for (int j = 0; j < i / 2; j++) {
        char temp = base64_str[j];
        base64_str[j] = base64_str[i - j - 1];
        base64_str[i - j - 1] = temp;
    }

    return base64_str;
}

size_t url_to_number(const char* base64_str) {
    size_t num = 0;
    size_t len = strlen(base64_str);

    for (size_t i = 0; i < len; i++) {
        unsigned char c = (unsigned char)base64_str[i];
        size_t value = useable_lookup[c];

        if (value == (size_t)-1) {
            return __SIZE_MAX__;
        }

        num = (num * 64) + value;
    }

    return num;
}
