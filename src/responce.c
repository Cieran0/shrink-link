#include "poll_loop.h"
#include "hash.h"

void get_request(int i, const char* path) {

    printf("Get request\n");

    int client_socket = socket_fds[i].fd;

    if(strcmp(path, "/")) {

        size_t index = url_to_number(path+1);

        if(index >= url_count) {
            const char* not_found = "HTTP/1.1 404 Not Found\nContent-Type: text/plain\nContent-Length: 13\n\n404 Not Found";
            send(client_socket, not_found, strlen(not_found), 0);
            return;
        }
        
        const char* redirect_location = stored_urls[index];
        

        char* redirect = generate_redirect(redirect_location);

        send(client_socket, redirect, strlen(redirect), 0);

        free(redirect);
        return;
    }

    // assert(dynamic_site.number_of_args == 1);
    // char* dynamic = render_dynamic_page(dynamic_site, "get_request");

    send(client_socket, static_site, strlen(static_site), 0);

}

void post_request(int i, const char* path, const char* content) {
    int client_socket = socket_fds[i].fd;

    size_t index = hashmap_insert(content);
    char* short_url = number_to_url(index);

    char* responce;
    asprintf(&responce, "{\"url\": \"%s\"}", short_url);
    printf("Post: short-url = %s\n", short_url);
    char* text;
    asprintf(&text, "HTTP/1.1 200 OK\nContent-Type: application/json\nContent-Length: %ld\n\n%s", strlen(responce),  responce);
    send(client_socket, text, strlen(text), 0);

    free(text);
    free(responce);

    free(short_url);
}


void respond_to_client(int i, const char* buffer, int number_of_bytes_received) {
    int client_socket = socket_fds[i].fd;

    char** lines;
    size_t len = split_lines(buffer, '\n', &lines);
    if(len < 1) {
        printf("Error no lines got???\n");
        return;
    }

    char* request = lines[0];
    char** request_split;
    size_t request_len = split_lines(request, ' ', &request_split);
    if(request_len < 1) {
        printf("Invalid HTTP Request\n");
        return;
    }


    if(request_len < 3) {
        printf("Invalid HTTP Request\n");
        FREE_ARRAY(request_split, request_len);
        return;
    }

    if (!strcmp(request_split[0], "GET")) {
        get_request(i, request_split[1]);
    } else if (!strcmp(request_split[0], "POST")) {
        post_request(i, request_split[1], lines[len-1]);
    } else {
        char text[] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 13\n\nHello, World!";
        send(client_socket, text, sizeof(text), 0);
    }

    FREE_ARRAY(lines, len);
    FREE_ARRAY(request_split, request_len);
} 