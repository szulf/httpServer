#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <regex.h>
#include "server.h"

void *handle_client(void *arg)
{
    int client_fd = *(int *)arg;
    char buffer[BUFFER_SIZE];
    
    int bytes_received;

    if ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0)) < 0)
    {
        perror("Error receiving");
        return NULL;
    }

    regex_t regex;
    regcomp(&regex, "^([^ ]*) /([^ ]*) HTTP/1.1", REG_EXTENDED);
    regmatch_t matches[3];

    printf("%s", buffer);

    if (regexec(&regex, buffer, 3, matches, 0) == 0)
    {
        //Getting the file path
        buffer[matches[2].rm_eo] = '\0';
        const char *encoded_file_path = buffer + matches[2].rm_so;
        char *file_path = url_decode(encoded_file_path);

        // Getting the request method
        buffer[matches[1].rm_eo] = '\0';
        const char *request_type = buffer + matches[1].rm_so;

        const char *file_type = get_file_type(get_file_extension(file_path));
        
        char *response = (char *) malloc(BUFFER_SIZE * sizeof(char));
        int response_len;

        int file_fd = open(file_path, O_RDONLY);
        if (file_fd != -1)
        {
            // Checking if the http request method is get and so on
            // Then ideally some cleaner solution than a bunch of ifs
            // Or at least ifs that go to certain methods
            // But that would equal a bunch of boilerplate i feel like

            snprintf(response, BUFFER_SIZE, 
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: %s\r\n"
                    "\r\n"
                    , file_type);
            response_len = strlen(response);
            
            int bytes_read;
            while ((bytes_read = read(file_fd, response + response_len, BUFFER_SIZE - response_len)))
            {
                response_len += bytes_read;
            }
        } else
        {
            snprintf(response, BUFFER_SIZE, 
                    "HTTP/1.1 404 Not Found\r\n"
                    "Content-Type: text/plain\r\n"
                    "\r\n"
                    );
            response_len = strlen(response);
        }

        if (send(client_fd, response, response_len, 0) < 0)
        {
            perror("Error sending the response");
            return NULL;
        }

        close(file_fd);
        free(response);
        response = NULL;
        free(file_path);
        file_path = NULL;
    }

    close(client_fd);
    regfree(&regex);

    return NULL;
}
