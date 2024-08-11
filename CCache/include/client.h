#ifndef CLIENT_H
#define CLIENT_H

#include <stdlib.h>
#include "config.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include "common.h"


// Unfortunatelly we need to maintain a ds to keep track of all the connected
// clients so we can cleanup at the shutdown... A linked list
typedef struct client_node {
        node_data_t *client_data;
        struct client_node *next;
} client_node_t;

void add_client_to_list(client_node_t **head, node_data_t *client_data);
void remove_client_from_list(client_node_t **head, node_data_t *client_data);
void cleanup_all_clients(client_node_t **head);
void handle_client_read(client_t *client, struct epoll_event *ev, int epoll_fd);
void handle_client_write(client_t *client, struct epoll_event *ev, int epoll_fd);

extern client_node_t *client_list_head;

#endif // CLIENT_H
