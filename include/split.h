#pragma once
#include "unistd.h"

size_t split_lines(const char* in, char delim, char*** out);