#include "../include/client.h"

void add_client_to_list(client_node_t **head, node_data_t *client_data) {
        client_node_t *new_node = malloc(sizeof(client_node_t));
        if (!new_node) {
                perror("malloc failed");
                return;
        }
        new_node->client_data = client_data;
        new_node->next = *head;
        *head = new_node;
}

// Remove client from the linked list
void remove_client_from_list(client_node_t **head, node_data_t *client_data) {
        printf("remove_client triggered\n");
        client_node_t *current = *head;
        client_node_t *prev = NULL;

        while (current != NULL) {
                if (current->client_data == client_data) {
                        if (prev == NULL) {
                                *head = current->next;
                        } else {
                                prev->next = current->next;
                        }
                        close(current->client_data->data.client->fd);
                        free(current->client_data->data.client);
                        current->client_data->data.client = NULL;
                        free(current->client_data);
                        current->client_data = NULL;
                        free(current);
                        current = NULL;
                        // free(current);
                        return;
                }
                prev = current;
                current = current->next;
        }
}

void cleanup_all_clients(client_node_t *head) {
        printf("Cleanup Triggered\n");
        client_node_t *current = head;
        while (current) {
                client_node_t *next = current->next;
                if (current->client_data && current->client_data->data.client) {
                        close(current->client_data->data.client->fd);
                        free(current->client_data->data.client);
                        current->client_data->data.client = NULL;
                }
                // close(current->client_data->data.client->fd);
                // free(current->client_data->data.client);
                // current->client_data->data.client = NULL;
                free(current->client_data);
                current->client_data = NULL;
                free(current);
                current = NULL;
                current = next;
        }
}

void handle_client_read(client_t *client, struct epoll_event *ev,
                        int epoll_fd) {
        char buffer[BUFFER_SIZE];
        int bytes_read = read(client->fd, buffer, sizeof(buffer));

        if (bytes_read > 0) {
                printf("Received message: %.*s\n", bytes_read, buffer);

                const char *response = "Hello World from Server\n";
                snprintf(client->buffer, sizeof(client->buffer), "%s",
                         response);
                client->buffer_len = strlen(client->buffer);
                client->buffer_pos = 0;

                // Enable write event and edge trigger to avoid multiple system
                // calls Changing the flags on what epoll should monitor, avoids
                // unwanted/random events to be triggered sporadically and makes
                // the system more robust
                ev->events =
                    EPOLLOUT | EPOLLET | EPOLLHUP | EPOLLERR | EPOLLRDHUP;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client->fd, ev) == -1) {
                        perror("Epoll ctl mod failed");
                        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->fd,
                                      NULL) == -1) {
                                perror("epoll_ctl: EPOLL_CTL_DEL failed");
                        }
                        // close(client->fd);
                        remove_client_from_list(&client_list_head,
                                                (node_data_t *)ev->data.ptr);
                        // free(client);
                        // client = NULL;
                        // free(ev->data.ptr);
                        // ev->data.ptr = NULL;
                }
        } else if (bytes_read == 0) {
                // Handle disconnect
                printf("Client disconnected: %p\n", client);
                if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->fd, NULL) ==
                    -1) {
                        perror("epoll_ctl: EPOLL_CTL_DEL failed");
                }
                // close(client->fd);
                remove_client_from_list(&client_list_head,
                                        (node_data_t *)ev->data.ptr);
                // free(client);
                // client = NULL;
                // free(ev->data.ptr);
                // ev->data.ptr = NULL;
        } else if (bytes_read < 0 && errno != EAGAIN) {
                // Handle read error
                perror("Read error");
                if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->fd, NULL) ==
                    -1) {
                        perror("epoll_ctl: EPOLL_CTL_DEL failed");
                }
                // close(client->fd);
                remove_client_from_list(&client_list_head,
                                        (node_data_t *)ev->data.ptr);
                // free(client);
                // client = NULL;
                // free(ev->data.ptr);
                // ev->data.ptr = NULL;
        }
}

void handle_client_write(client_t *client, struct epoll_event *ev,
                         int epoll_fd) {
        while (client->buffer_pos < client->buffer_len) {
                int bytes_written =
                    write(client->fd, client->buffer + client->buffer_pos,
                          client->buffer_len - client->buffer_pos);
                if (bytes_written < 0) {
                        if (errno == EAGAIN) {
                                // Socket not ready for writing, retry later
                                ev->events |= EPOLLOUT;
                                epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client->fd,
                                          ev);
                                return;
                        } else {
                                perror("Write error");
                                if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL,
                                              client->fd, NULL) == -1) {
                                        perror(
                                            "epoll_ctl: EPOLL_CTL_DEL failed");
                                }
                                // close(client->fd);
                                remove_client_from_list(
                                    &client_list_head,
                                    (node_data_t *)ev->data.ptr);
                                // free(client);
                                // client = NULL;
                                // free(ev->data.ptr);
                                // ev->data.ptr = NULL;

                                return;
                        }
                }
                client->buffer_pos += bytes_written;
        }

        // Stop monitoring for writable events
        ev->events = EPOLLIN | EPOLLET | EPOLLHUP | EPOLLERR | EPOLLRDHUP;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client->fd, ev) == -1) {
                perror("Epoll ctl mod failed");
                if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->fd, NULL) ==
                    -1) {
                        perror("epoll_ctl: EPOLL_CTL_DEL failed");
                }
                // close(client->fd);
                remove_client_from_list(&client_list_head,
                                        (node_data_t *)ev->data.ptr);
                // free(client);
                // client = NULL;
                // free(ev->data.ptr);
                // ev->data.ptr = NULL;
        }
}
