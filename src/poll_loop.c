#include "poll_loop.h"

int active = 1;

void listen_for_connections()
{
    if (socket_fds[0].revents & POLLIN)
    {
        struct sockaddr_in6 client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket_fd, (struct sockaddr*)&client_addr, &client_addr_len);

        if (client_socket < 0)
        {
            printf("Error accepting client\n");
            return;
        }

        if (number_of_socket_fds < MAX_CLIENTS)
        {
            socket_fds[number_of_socket_fds].fd = client_socket;
            socket_fds[number_of_socket_fds].events = POLLIN; 
            number_of_socket_fds++;

            char ip_buffer[INET6_ADDRSTRLEN] = {0};
            inet_ntop(AF_INET6, &client_addr.sin6_addr, ip_buffer, sizeof(ip_buffer));

            printf("New connection from IP: [%s]\n", ip_buffer);
        }
        else
        {
            printf("Too many clients\n");
            close(client_socket);
        }
    }
}

void kill_client_connection(int client_socket, int i) {
    close(client_socket);

    // If not the last client, move the last client to i.
    if (i != number_of_socket_fds - 1) {
        socket_fds[i].fd = socket_fds[number_of_socket_fds - 1].fd;
        socket_fds[i].events = socket_fds[number_of_socket_fds - 1].events;
    }
    number_of_socket_fds--;
    printf("Closed connection with client socket: %d", client_socket);
}



void listen_to_client(int i) {
    const size_t buffer_length = 1025;
    char buffer[buffer_length];
    int client_socket = socket_fds[i].fd;
    int number_of_bytes_received = recv(client_socket, buffer, buffer_length - 1, 0);

    // Connection is alive
    if (number_of_bytes_received > 0) {
        buffer[number_of_bytes_received] = '\0';
        respond_to_client(i, buffer, number_of_bytes_received+1);
        return;
    }

    if (number_of_bytes_received == 0) {
        printf("Client disconnected\n");
    } 
    else { 
        printf("Socket error on client: %d\n", client_socket);
    }
    kill_client_connection(client_socket, i);


}

void handle_socket_error(int i) {
    int client_socket = socket_fds[i].fd;
    printf("Error or hang-up on socket: %d\n", client_socket);
    kill_client_connection(client_socket, i);
}

void handle_client_sockets()
{
    for (int i = 1; i < number_of_socket_fds; ++i)
    {
        if (socket_fds[i].revents & POLLIN) {
            //Client has sent data
            listen_to_client(i);
        }
        else if (socket_fds[i].revents & (POLLHUP | POLLERR | POLLNVAL)) {
            //Error on the socket
            handle_socket_error(i);
        }
    }
}

void poll_loop() {
    while (active)
    {
        int poll_count = poll(socket_fds, number_of_socket_fds, 5000);

        listen_for_connections();
        handle_client_sockets();

        if(poll_count < 0) {
            printf("Poll Error Occurred, Dying\n");
            break;
        }

    }

}