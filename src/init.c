#include "main.h"
#include "hash.h"

FILE* load_stored_map() {
    const char* path = "map.txt";

    FILE* fd = fopen(path, "r");
    if (fd == NULL) {
        printf("%s: Doesn't exist, attempting to create it.\n", path);
        fd = fopen(path, "w");
        if (fd == NULL) {
            printf("Failed to create %s\n", path);
            return NULL;
        }
        fclose(fd);
        return fopen(path, "w");
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

    buffer[bytes_read] = '\0';
    
    char** long_urls;
    size_t len = split_lines(buffer, '\n', &long_urls);
    printf("%s: Found %ld Stored URLs\n", path, len);
    fclose(fd);

    stored_map_file = fopen(path, "w");
    if(stored_map_file == NULL) {
        FREE_ARRAY(long_urls, len)
        free(buffer);
        return NULL;
    }

    for (size_t i = 0; i < len; i++)
    {
        hashmap_insert(long_urls[i]);
    }

    FREE_ARRAY(long_urls, len)
    free(buffer);

    return stored_map_file;
}

int init() {

    stored_urls_arena = new_arena();
    stored_urls = stored_urls_arena->start;

    init_url_char_lookup_table();
    stored_map_file = load_stored_map();

    if(stored_map_file == NULL) {
        printf("Error: Failed to read stored map\n");
        return EXIT_FAILURE;
    }

    static_site = load_static_page("index.html");

    if(static_site == NULL) {
        printf("Failed to read static page!\n");
        return EXIT_FAILURE;
    }

    // dynamic_site = load_dynamic_page("index.html");

    // if(dynamic_site.src == NULL) {
    //     printf("Failed to read dynamic page!\n");
    //     return EXIT_FAILURE;
    // }

    server_socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
    if(server_socket_fd < 0){
        printf("Error creating socket\n");
        return EXIT_FAILURE;
    }

    int optval = 1;
    if(setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval)) < 0){
        printf("Error setting socket options\n");
        close(server_socket_fd);
        return EXIT_FAILURE;
    }

    memset(&address,0,sizeof(address));
    address.sin6_family = AF_INET6;
    address.sin6_addr = in6addr_any;
    address.sin6_port = htons(port);

    if(bind(server_socket_fd, (struct sockaddr*)&address,sizeof(address))<0){
        printf("Error binding socket\n");
        close(server_socket_fd);
        return EXIT_FAILURE;
    }

    if(listen(server_socket_fd, 3)<0){
        printf("Error listening on socket");
        close(server_socket_fd);
        return EXIT_FAILURE;
    }

    printf("Server started on port: %ld (IPv6)\n",port);

    socket_fds[0].fd = server_socket_fd;
    socket_fds[0].events = POLLIN;

    return 0;
}