#ifndef SERVER_H
#define SERVER_H

#define BUFFER_SIZE 10000
#define PORT 8080

void *handle_client(void*);
char *url_decode(const char*);
char *get_file_extension(const char*);
char *get_file_type(const char*);

#endif