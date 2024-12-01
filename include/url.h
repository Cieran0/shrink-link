#pragma once
#include "stdlib.h"
#include "unistd.h"


char* number_to_url(size_t num);
size_t url_to_number(const char* base64_str);
void init_url_char_lookup_table();