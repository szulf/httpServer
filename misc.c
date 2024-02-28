#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "server.h"

char *url_decode(const char *encoded_url)
{
    int encoded_url_len = strlen(encoded_url);
    char *decoded_url = malloc(encoded_url_len * sizeof(char) + 1);
    int decoded_url_len = 0;

    for (int i = 0; i < encoded_url_len; i++)
    {
        if (encoded_url[i] == '%' && i + 2 < encoded_url_len)
        {
            int hex_val;
            sscanf(encoded_url + i + 1, "%2x", &hex_val); // Getting the 2 characters after '%' in url
            // Turns out the %20 in the url is space because 20 hex in ascii is space
            decoded_url[decoded_url_len] = hex_val;
            decoded_url_len++;
            i += 2;
        }
        else
        {
            decoded_url[decoded_url_len] = encoded_url[i];
            decoded_url_len++;
        }
    }
    decoded_url[decoded_url_len] = '\0';
    return decoded_url;
}

char *get_file_extension(const char *file_path)
{
    char *file_extension = strrchr(file_path, '.');
    if (file_extension == NULL || file_extension == file_path)
        return "";
    return file_extension + 1;
}

char *get_file_type(const char *extension)
{
    if (strcmp(extension, "html") == 0)
        return "text/html";
    else if (strcmp(extension, "txt") == 0)
        return "text/plain";
    else if (strcmp(extension, "jpeg") == 0)
        return "image/jpeg";
    return "application/octet-stream";
}

void build_response_body(int file_fd, char *response_body, int *response_body_len)
{
    int bytes_read;
    while ((bytes_read = read(file_fd, response_body + *response_body_len, BUFFER_SIZE - *response_body_len)))
    {
        *response_body_len += bytes_read;
    }
}

void build_response(const char *content_type, int content_length, const char *response_body, char *response, int *response_len)
{
    snprintf(response, BUFFER_SIZE, 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %d\r\n"
            "\r\n"
            "%s"
            , content_type, content_length, response_body);
    *response_len = strlen(response);
}

void build_404_response(char *response, int *response_len)
{
    snprintf(response, BUFFER_SIZE, 
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n"
            "\r\n"
            "404 Not found"
            );
    *response_len = strlen(response);
}
