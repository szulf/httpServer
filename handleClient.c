#include "server.h"
#include <fcntl.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void *handle_client(void *arg) {
  int client_fd = *(int *)arg;
  char buffer[BUFFER_SIZE];

  int bytes_received;

  if ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0)) < 0) {
    perror("Error receiving");
    return NULL;
  }

  regex_t regex;
  regcomp(&regex, "^([^ ]*) /([^ ]*) HTTP/1.1", REG_EXTENDED);
  regmatch_t matches[3];

  printf("%s", buffer);

  if (regexec(&regex, buffer, 3, matches, 0) == 0) {
    // Getting the file path
    buffer[matches[2].rm_eo] = '\0';
    const char *encoded_file_path = buffer + matches[2].rm_so;
    char *file_path = url_decode(encoded_file_path);

    // Getting the request method
    buffer[matches[1].rm_eo] = '\0';
    const char *request_method = buffer + matches[1].rm_so;

    const char *file_type = get_file_type(get_file_extension(file_path));

    char *response = (char *)malloc(BUFFER_SIZE * sizeof(char));
    int response_len = 0;
    char *response_body =
        (char *)malloc((BUFFER_SIZE - HEADER_SIZE) * sizeof(char));
    int response_body_len = 0;

    int file_fd = open(file_path, O_RDONLY);
    if (file_fd != -1) {
      // Checking if the http request method is get or post
      if (strcmp(request_method, "GET") == 0) {
        build_response_body(file_fd, response_body, &response_body_len);
        build_response(file_type, response_body_len, response_body, response, &response_len);

      } else if (strcmp(request_method, "POST") == 0) {
        // build_post_reponse();
        //  Not implemented yet
        //  No idea how to do it or how it works
        //  To be fair this doesn't really make sense with the way the server
        //  works now Since even if you passed any variables using a form, there
        //  is no way of doing anything with them Because the server doesn't
        //  process any code like PHP for example And I don't feel like
        //  implementing any interpreter for that :D
      }
    } else {
      build_404_response(response, &response_len);
    }

    if (send(client_fd, response, response_len, 0) < 0) {
      perror("Error sending the response");
      exit(0);
    }

    close(file_fd);
    free(response);
    response = NULL;
    free(response_body);
    response_body = NULL;
    free(file_path);
    file_path = NULL;
  }

  close(client_fd);
  regfree(&regex);

  return NULL;
}
