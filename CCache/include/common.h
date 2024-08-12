#ifndef COMMON_H
#define COMMON_H

#include "config.h"
#include <unistd.h>

typedef struct {
    int fd;
    char read_buffer[BUFFER_SIZE];
    int read_buffer_len;
    char write_buffer[BUFFER_SIZE];
    int write_buffer_len;
    int write_buffer_pos;
    int close_after_write;
} client_t;

typedef struct {
        int node_type; // 0 for server and 1 for client
        union {
                int server_fd;    // Use this if server
                client_t *client; // Use this if client
        } data;
} node_data_t;

#endif // COMMON_H
