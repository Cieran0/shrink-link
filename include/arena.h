#pragma once
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#define ARENA_BLOCK_SIZE (sizeof(char*) * 512)

struct arena
{
    void* start;
    void* next;
    size_t size_in_bytes;
};

struct arena* new_arena();
void* arena_alloc(struct arena* arena, size_t size);
void arena_free(struct arena* arena);