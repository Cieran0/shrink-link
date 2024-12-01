#include "main.h"
#include "poll_loop.h"

size_t port = 80;

size_t server_socket_fd = 0;
struct pollfd socket_fds[MAX_CLIENTS];
size_t number_of_socket_fds = 1;
struct sockaddr_in6 address = {};

const char* static_site;
struct dynamic_page dynamic_site;

struct arena* stored_urls_arena;
const char** stored_urls;

FILE* stored_map_file;

int main(int argc, char const *argv[])
{
    if (geteuid() != 0) {
        port = 8000;
        printf("Run as root for default HTTP port (80)\n");
    }

    if(init() == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    poll_loop();
    
    return 0;
}
