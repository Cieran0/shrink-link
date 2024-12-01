#pragma once
#include <stdarg.h>


struct dynamic_page {
    const char* src;
    size_t number_of_args;
};

char* load_static_page(const char* path);
struct dynamic_page load_dynamic_page(const char* path);
char* render_dynamic_page(struct dynamic_page page, ...);