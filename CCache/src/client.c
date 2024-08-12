#include "../include/client.h"

#include <stdio.h>

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
    client_node_t *current = *head;
    client_node_t *prev = NULL;

    while (current != NULL) {
        if (current->client_data == client_data) {
            if (prev == NULL) {
                *head = current->next;
            } else {
                prev->next = current->next;
            }
            if (current->client_data && current->client_data->data.client) {
                close(current->client_data->data.client->fd);
                free(current->client_data->data.client);
                current->client_data->data.client = NULL;
            }
            if (current->client_data) {
                free(current->client_data);
                current->client_data = NULL;
                current->next = NULL;
            }
            free(current);
            current = NULL;
            return;
        }
        prev = current;
        current = current->next;
    }
}

void cleanup_all_clients(client_node_t **head) {
    printf("Cleanup Triggered\n");
    client_node_t *current = *head;
    while (current) {
        client_node_t *next = current->next;
        if (current->client_data && current->client_data->data.client) {
            close(current->client_data->data.client->fd);
            free(current->client_data->data.client);
            current->client_data->data.client = NULL;
        }
        if (current->client_data) {
            free(current->client_data);
            current->client_data = NULL;
            current->next = NULL;
        }
        free(current);
        current = next;
    }
    *head = NULL;
}

void delete_resources(int epoll_fd, client_t *client, struct epoll_event *ev) {
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->fd, NULL) == -1) {
        perror("epoll_ctl: EPOLL_CTL_DEL failed");
    }

    remove_client_from_list(&client_list_head, (node_data_t *)ev->data.ptr);
}

void set_error_msg(int epoll_fd, client_t *client, struct epoll_event *ev,
                   const char *error_response) {
    int response_len = strlen(error_response);

    if (client->write_buffer_len + response_len < BUFFER_SIZE) {
        memcpy(client->write_buffer + client->write_buffer_len, error_response,
               response_len);
        client->write_buffer_len += response_len;

        client->close_after_write = 1;

        ev->events = EPOLLOUT | EPOLLET | EPOLLHUP | EPOLLERR | EPOLLRDHUP;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client->fd, ev) == -1) {
            perror("Epoll ctl mod failed");
            delete_resources(epoll_fd, client, ev);
        }
    } else {
        fprintf(stderr, "Write buffer overflow\n");
        delete_resources(epoll_fd, client, ev);
    }
}

void handle_client_read(client_t *client, struct epoll_event *ev,
                        int epoll_fd) {
    char temp_buffer[BUFFER_SIZE];
    int bytes_read = read(client->fd, temp_buffer, sizeof(temp_buffer));

    printf("Received message: %.*s\n", bytes_read, temp_buffer);

    if (bytes_read > 0) {
        // Accumulate read data
        if (client->read_buffer_len + bytes_read <= BUFFER_SIZE) {
            memcpy(client->read_buffer + client->read_buffer_len, temp_buffer,
                   bytes_read);
            client->read_buffer_len += bytes_read;

            // Ensure the string is terminated
            if (client->read_buffer_len < BUFFER_SIZE) {
                client->read_buffer[client->read_buffer_len] = '\0';
            } else {
                client->read_buffer[BUFFER_SIZE - 1] = '\0';
            }

            // Check if the buffer contains at least one \r\n
            char *first_rn = strstr(client->read_buffer, "\r\n");

            // If a \r\n exists, ensure there's only one and process the command
            if (first_rn != NULL) {
                // Check if there is any data after the first \r\n
                if (first_rn + 2 <
                    client->read_buffer + client->read_buffer_len) {
                    // There's more data after the first \r\n, this as an error
                    const char *error_response =
                        "ERROR: Multiple commands in one "
                        "request\r\n";
                    set_error_msg(epoll_fd, client, ev, error_response);
                    return;
                }

                // Happy path
                int command_length = first_rn - client->read_buffer;
                char command[BUFFER_SIZE];
                strncpy(command, client->read_buffer, command_length);
                command[command_length] = '\0';

                printf("Processing command: %s\n", command);
                const char *response = "Hello World from Server\r\n";
                int response_len = strlen(response);

                // Check if the write buffer has enough space for the response
                if (client->write_buffer_len + response_len < BUFFER_SIZE) {
                    memcpy(client->write_buffer + client->write_buffer_len,
                           response, response_len);
                    client->write_buffer_len += response_len;
                } else {
                    fprintf(stderr, "Write buffer overflow\n");
                    delete_resources(epoll_fd, client, ev);
                    return;
                }

                client->read_buffer_len = 0;

                // Change the event to monitor for writing
                ev->events =
                    EPOLLOUT | EPOLLET | EPOLLHUP | EPOLLERR | EPOLLRDHUP;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client->fd, ev) == -1) {
                    perror("Epoll ctl mod failed");
                    delete_resources(epoll_fd, client, ev);
                    return;
                }

            } else if (client->read_buffer_len == BUFFER_SIZE) {
                // Buffer is full but no complete command, this is an error
                const char *error_response =
                    "ERROR: Command too large or incomplete\r\n";
                set_error_msg(epoll_fd, client, ev, error_response);
                return;
            }
        } else {
            // Buffer overflow, the command is too large
            const char *error_response = "ERROR: Command too large\r\n";
            set_error_msg(epoll_fd, client, ev, error_response);
            return;
        }

    } else if (bytes_read == 0) {
        // Handle disconnect
        printf("Client disconnected: %p\n", client);
        delete_resources(epoll_fd, client, ev);
    } else if (bytes_read < 0 && errno != EAGAIN) {
        // Handle read error
        perror("Read error");
        delete_resources(epoll_fd, client, ev);
    }
}

void handle_client_write(client_t *client, struct epoll_event *ev,
                         int epoll_fd) {
    while (client->write_buffer_pos < client->write_buffer_len) {
        int bytes_written =
            write(client->fd, client->write_buffer + client->write_buffer_pos,
                  client->write_buffer_len - client->write_buffer_pos);
        if (bytes_written < 0) {
            if (errno == EAGAIN) {
                // Socket not ready for writing, retry later

                ev->events |= EPOLLOUT;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client->fd, ev) == -1) {
                    perror("epoll_ctl: EPOLL_CTL_MOD failed");
                    delete_resources(epoll_fd, client, ev);
                }
                return;
            } else {
                perror("Write error");
                delete_resources(epoll_fd, client, ev);
                return;
            }
        }
        client->write_buffer_pos += bytes_written;
    }

    // Stop monitoring for writable events
    if (client->write_buffer_pos == client->write_buffer_len) {
        client->write_buffer_pos = 0;

        client->write_buffer_len = 0;
        ev->events = EPOLLIN | EPOLLET | EPOLLHUP | EPOLLERR | EPOLLRDHUP;

        if (client->close_after_write) {
            delete_resources(epoll_fd, client, ev);
            return;
        }

        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client->fd, ev) == -1) {
            perror("Epoll ctl mod failed");
            delete_resources(epoll_fd, client, ev);
            return;
        }
    }
}
