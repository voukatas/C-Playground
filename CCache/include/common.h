#ifndef COMMON_H
#define COMMON_H

#include "config.h"
#include <unistd.h>

typedef struct {
        int fd;
        char buffer[BUFFER_SIZE];
        size_t buffer_len;
        size_t buffer_pos;
} client_t;

typedef struct {
        int node_type; // 0 for server and 1 for client
        union {
                int server_fd;    // Use this if server
                client_t *client; // Use this if client
        } data;
} node_data_t;

#endif // COMMON_H
