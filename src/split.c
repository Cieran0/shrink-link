#include "split.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"

bool try_alloc_lines(char** lines, int i, size_t size) {
    lines[i] = malloc(size);
    if (!lines[i]) {
        for (size_t j = 0; j < i; j++) {
            free(lines[j]);
        }
        free(lines);
        return false;
    }

    return true;
}

size_t split_lines(const char* in, char delim, char*** out) {
    if (!in || !out) {
        return 0;
    }

    size_t count = 0;
    const char* start = in;
    const char* current = in;

    while (*current) {
        if (*current == delim) {
            count++;
        }
        current++;
    }
    if (current > start) {
        count++;
    }

    if(*(current-1) == delim) {
        count--;
    }

    char** lines = malloc(sizeof(char*) * count);
    if (!lines) {
        return 0;
    }

    current = in;
    start = in;
    size_t i = 0;

    while (*current) {
        if (*current == delim) {
            size_t size = current - start + 1;
            if(!try_alloc_lines(lines, i, size))
                return 0;
            strncpy(lines[i], start, size - 1);
            lines[i][size - 1] = '\0';
            start = current + 1;
            i++;
        }
        current++;
    }

    if (start < current) {
        size_t size = strlen(start) + 1;
        if(!try_alloc_lines(lines, i, size))
            return 0;
        strncpy(lines[i], start, size - 1);
    }

    *out = lines;
    return count;
}