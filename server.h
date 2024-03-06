#ifndef SERVER_H
#define SERVER_H

#define BUFFER_SIZE 10000
#define HEADER_SIZE 1000
#define PORT 8080

void *handle_client(void*);
char *url_decode(const char*);
char *get_file_extension(const char*);
char *get_file_type(const char*);
void build_response_body(int, char *, int *);
void build_response(const char*, int, const char*, char*, int*);
void build_404_response(char *, int *);

#endif