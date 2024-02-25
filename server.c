#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include "server.h"

int main(int argc, char **argv)
{

    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(PORT);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1)
    {
        perror("Error creating a socket");
        exit(0);
    }

    int yes = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
    {
        perror("setsockopt() error");
        exit(0);
    }

    if (bind(server_fd, (struct sockaddr*) &my_addr, sizeof(my_addr)) < 0)
    {
        perror("Error binding socket");
        exit(0);
    }

    if (listen(server_fd, 50) < 0)
    {
        perror("Error listening");
        exit(0);
    }

    while (1)
    {
        struct sockaddr_in client_info;
        socklen_t client_info_size = sizeof(client_info);
        int client_fd;

        if ((client_fd = accept(server_fd, (struct sockaddr*) &client_info, &client_info_size)) < 0)
        {
            perror("Error accepting client");
            exit(0);
        }

        pthread_t client_thread;
        pthread_create(&client_thread, 0, handle_client, &client_fd);
        pthread_detach(client_thread);
    }

    if (close(server_fd) < 0)
    {
        perror("Error closing the socket");
        exit(0);
    }

    return 0;
}
