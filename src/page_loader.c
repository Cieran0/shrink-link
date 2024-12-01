#define _GNU_SOURCE

#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"
#include "static_loader.h"
#include "string.h"

size_t count_format_specifiers(const char* fmt) {
    size_t count = 0;

    if (!fmt) return 0; // Null pointer check

    while (*fmt) {
        if (*fmt == '%') {
            ++fmt;

            if (*fmt == '%') {
                ++fmt;
                continue;
            }

            // Check if the next character is a valid format specifier
            if (*fmt == 'd' || *fmt == 'i' || *fmt == 'u' || *fmt == 'f' || 
                *fmt == 's' || *fmt == 'c' || *fmt == 'x' || *fmt == 'X' ||
                *fmt == 'o' || *fmt == 'p' || *fmt == 'e' || *fmt == 'E' || 
                *fmt == 'g' || *fmt == 'G' || *fmt == 'a' || *fmt == 'A' || 
                *fmt == 'n') {
                ++count; // Valid specifier found
                ++fmt;   // Move past the specifier
            } else if (*fmt) {
                // Invalid specifier, skip this and treat as non-format
                ++fmt;
            }
        } else {
            ++fmt;
        }
    }

    return count;
}

char* load_static_page(const char* path) {
    FILE* fd = fopen(path, "r");
    if(fd == NULL) {
        return NULL;
    }

    fseek(fd, 0, SEEK_END);
    size_t file_size = ftell(fd);
    rewind(fd);

    char *buffer = (char *)malloc(file_size + 1);
    if (buffer == NULL) {
        fclose(fd);
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, file_size, fd);
    if (bytes_read != file_size) {
        free(buffer);
        fclose(fd);
        return NULL;
    }

    buffer[file_size] = '\0';
    fclose(fd);

    char* output;
    asprintf(&output, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: %ld\n\n%s", file_size, buffer);
    free(buffer);

    return output;
}

struct dynamic_page load_dynamic_page(const char* path) {
    const struct dynamic_page FAILED = {.src = NULL, .number_of_args = -1 };

    FILE* fd = fopen(path, "r");
    if(fd == NULL) {
        return FAILED;
    }

    fseek(fd, 0, SEEK_END);
    size_t file_size = ftell(fd);
    rewind(fd);

    char *buffer = (char *)malloc(file_size + 1);
    if (buffer == NULL) {
        fclose(fd);
        return FAILED;
    }

    size_t bytes_read = fread(buffer, 1, file_size, fd);
    if (bytes_read != file_size) {
        free(buffer);
        fclose(fd);
        return FAILED;
    }

    buffer[file_size] = '\0';
    fclose(fd);

    struct dynamic_page dynamic_output = {.src = buffer, .number_of_args = count_format_specifiers(buffer)};

    return dynamic_output;
}

char* render_dynamic_page(struct dynamic_page page, ...) {
    va_list args;
    va_start(args, page);


    char* result = NULL;
    if (vasprintf(&result, page.src, args) == -1) {
        result = NULL;
    }

    va_end(args);

    char* output;
    asprintf(&output, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: %ld\n\n%s", strlen(result), result);
    free(result);

    return output;
}
#undef _GNU_SOURCE