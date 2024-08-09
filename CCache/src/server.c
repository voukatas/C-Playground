#include "../include/server.h"
#include "../include/client.h"
#include "../include/config.h"
#include "../include/signal_handler.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>

client_node_t *client_list_head = NULL;
node_data_t *server_event = NULL;

void set_non_blocking(int socket) {
        int flags = fcntl(socket, F_GETFL, 0);
        fcntl(socket, F_SETFL, flags | O_NONBLOCK);
}

int setup_server_socket(int port) {
        int server_fd; //, client_socket;
        struct sockaddr_in address = {0};

        // Create a socket
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == 0) {
                perror("Socket failed");
                exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        // Set the reuse flag
        int reuse = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse,
                       sizeof(int)) == -1) {
                perror("Set reuse failed");
                close(server_fd);
                exit(EXIT_FAILURE);
        }
        // Bind
        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
                perror("Bind failed");
                close(server_fd);
                exit(EXIT_FAILURE);
        }

        // Listen
        if (listen(server_fd, 3) < 0) {
                perror("Listen failed");
                close(server_fd);
                exit(EXIT_FAILURE);
        }

        set_non_blocking(server_fd);

        return server_fd;
}

int setup_epoll(int server_fd) {
        int epoll_fd = epoll_create1(0); // a kernel obj that keeps track of
                                         // multiple fds and notifies on events
        if (epoll_fd == -1) {
                perror("Epoll create failed");
                close(server_fd);
                exit(EXIT_FAILURE);
        }

        server_event = malloc(sizeof(node_data_t));
        if (!server_event) {
                perror("malloc failed");
                close(server_fd);
                exit(EXIT_FAILURE);
        }
        server_event->node_type = 0;
        server_event->data.server_fd = server_fd;

        struct epoll_event ev;
        ev.events =
            EPOLLIN; // set flag to listen for read events on server socket
        ev.data.ptr = server_event;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
                perror("Epoll ctl add failed");
                close(server_fd);
                close(epoll_fd);
                free(server_event);
                server_event = NULL;
                exit(EXIT_FAILURE);
        }

        return epoll_fd;
}

void handle_event(int epoll_fd, struct epoll_event *event) {
        struct sockaddr_in address;
        int addrlen = sizeof(address);
        node_data_t *event_data = (node_data_t *)event->data.ptr;

        if (event_data->node_type == 0) {
                // Accept a new client connection
                int client_socket =
                    accept(event_data->data.server_fd,
                           (struct sockaddr *)&address, (socklen_t *)&addrlen);
                if (client_socket < 0) {
                        perror("Accept failed");
                        return;
                }
                printf("Accepted client socket\n");

                set_non_blocking(client_socket);

                client_t *client = malloc(sizeof(client_t));
                if (!client) {
                        perror("malloc failed");
                        close(client_socket);
                        return;
                }
                printf("Accepted client socket 1\n");

                client->fd = client_socket;
                client->buffer_len = 0;
                client->buffer_pos = 0;

                node_data_t *client_event = malloc(sizeof(node_data_t));
                if (!client_event) {
                        perror("malloc failed");
                        close(client_socket);
                        free(client);
                        client = NULL;
                        return;
                }
                client_event->node_type = 1;
                client_event->data.client = client;

                printf("Accepted client socket 2\n");

                // Add the client socket to epoll
                struct epoll_event ev;
                ev.events = EPOLLIN | EPOLLET;
                ev.data.ptr = client_event;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &ev) ==
                    -1) {
                        perror("Epoll ctl add failed");
                        close(client_socket);
                        free(client);
                        client = NULL;
                        free(client_event);
                        client_event = NULL;
                        return;
                }

                printf("Accepted client socket 3\n");

                // Add client to the linked list for cleanup on
                // shutdown
                add_client_to_list(&client_list_head, client_event);
                printf("added client %p\n", client);
        } else {
                // Handle client disconnect or error
                client_t *client = event_data->data.client;

                if (event->events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP)) {
                        printf("Disconnect event for %p\n",
                               event_data->data.client);
                        // Remove the FD from epoll
                        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->fd,
                                      NULL) == -1) {
                                perror("epoll_ctl: "
                                       "EPOLL_CTL_DEL failed");
                        }
                        // close(client->fd);
                        // free(client);
                        // client = NULL;
                        remove_client_from_list(&client_list_head, event_data);
                        // free(event_data);
                        // event_data = NULL;
                        return;
                }

                // Handle the client request
                if (event->events & EPOLLIN) {
                        printf("Ready to read\n");
                        handle_client_read(client, event, epoll_fd);
                }
                if (event->events & EPOLLOUT) {
                        printf("Ready to write\n");
                        handle_client_write(client, event, epoll_fd);
                }
        }
}

int run_server(int port) {
        // Set up signal handling
        setup_signal_handling();
        int server_fd = setup_server_socket(port);
        int epoll_fd = setup_epoll(server_fd);

        printf("Server is listening on port %d\n", port);

        struct epoll_event events[MAX_EVENTS];

        // Event Loop
        while (keep_running) {
                int nfds = epoll_wait(
                    epoll_fd, events, MAX_EVENTS,
                    -1); // returns as soon as an event occurs, no delay
                if (nfds == -1) {
                        if (errno == EINTR) {
                                break;
                        } else {
                                perror("epoll_wait failed");
                                break;
                        }
                }

                for (int i = 0; i < nfds; ++i) {
                        handle_event(epoll_fd, &events[i]);
                }
        }
        // Cleanup on shutdown
        cleanup_all_clients(client_list_head);
        free(server_event);
        server_event = NULL;
        close(epoll_fd);
        close(server_fd);
        return 0;
}