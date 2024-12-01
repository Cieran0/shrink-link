#pragma once

#define _GNU_SOURCE
#include "stdio.h"
#undef _GNU_SOURCE
#include "poll.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "unistd.h"
#include "arpa/inet.h"
#include "stdbool.h"
#include "string.h"
#include "split.h"
#include "redirect.h"
#include "static_loader.h"
#include "stdlib.h"
#include "assert.h"
#include "url.h"
#include "arena.h"
#define MAX_CLIENTS 1024
#define FREE_ARRAY(array, size) for (size_t i = 0; i < size; i++) { free(array[i]); } free(array);

extern size_t port;
extern size_t server_socket_fd;
extern struct pollfd socket_fds[MAX_CLIENTS];
extern size_t number_of_socket_fds;
extern struct sockaddr_in6 address;

extern const char* static_site;
extern struct dynamic_page dynamic_site;

extern struct arena* stored_urls_arena;
extern const char** stored_urls;

extern size_t url_count;
extern FILE* stored_map_file;

int init();