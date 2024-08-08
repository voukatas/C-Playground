#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#define PORT 8080
#define MAX_EVENTS 64
#define BUFFER_SIZE 1024

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

// Unfortunatelly we need to maintain a ds to keep track of all the connected
// clients so we can cleanup at the shutdown... A linked list
typedef struct client_node {
  node_data_t *client_data;
  struct client_node *next;
} client_node_t;

client_node_t *client_list_head = NULL;
volatile sig_atomic_t keep_running = 1;

void handle_signal() { keep_running = 0; }

void set_non_blocking(int socket) {
  int flags = fcntl(socket, F_GETFL, 0);
  fcntl(socket, F_SETFL, flags | O_NONBLOCK);
}

// Add client to the linked list
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
      free(current);
      return;
    }
    prev = current;
    current = current->next;
  }
}

void cleanup_all_clients(client_node_t *head) {
  client_node_t *current = head;
  while (current) {
    client_node_t *next = current->next;
    close(current->client_data->data.client->fd);
    free(current->client_data->data.client);
    current->client_data->data.client = NULL;
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

    const char *response = "Hello World\n";
    client->buffer_len = strlen(response);
    strncpy(client->buffer, response, client->buffer_len);
    client->buffer_pos = 0;

    // Enable write event and edge trigger to avoid multiple system calls
    // Changing the flags on what epoll should monitor, avoids unwanted/random
    // events to be triggered sporadically and makes the system more robust
    ev->events = EPOLLOUT | EPOLLET;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client->fd, ev) == -1) {
      perror("Epoll ctl mod failed");
      if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->fd, NULL) == -1) {
        perror("epoll_ctl: EPOLL_CTL_DEL failed");
      }
      close(client->fd);
      remove_client_from_list(&client_list_head, (node_data_t *)ev->data.ptr);
      free(client);
      client = NULL;
      free(ev->data.ptr);
      ev->data.ptr = NULL;
    }
  } else if (bytes_read == 0) {
    // Handle disconnect
    printf("Client disconnected: %p\n", client);
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->fd, NULL) == -1) {
      perror("epoll_ctl: EPOLL_CTL_DEL failed");
    }
    close(client->fd);
    remove_client_from_list(&client_list_head, (node_data_t *)ev->data.ptr);
    free(client);
    client = NULL;
    free(ev->data.ptr);
    ev->data.ptr = NULL;
  } else if (bytes_read < 0 && errno != EAGAIN) {
    // Handle read error
    perror("Read error");
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->fd, NULL) == -1) {
      perror("epoll_ctl: EPOLL_CTL_DEL failed");
    }
    close(client->fd);
    remove_client_from_list(&client_list_head, (node_data_t *)ev->data.ptr);
    free(client);
    client = NULL;
    free(ev->data.ptr);
    ev->data.ptr = NULL;
  }
}

void handle_client_write(client_t *client, struct epoll_event *ev,
                         int epoll_fd) {
  while (client->buffer_pos < client->buffer_len) {
    int bytes_written = write(client->fd, client->buffer + client->buffer_pos,
                              client->buffer_len - client->buffer_pos);
    if (bytes_written < 0) {
      if (errno == EAGAIN) {
        // Socket not ready for writing, retry later
        ev->events |= EPOLLOUT;
        epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client->fd, ev);
        return;
      } else {
        perror("Write error");
        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->fd, NULL) == -1) {
          perror("epoll_ctl: EPOLL_CTL_DEL failed");
        }
        close(client->fd);
        remove_client_from_list(&client_list_head, (node_data_t *)ev->data.ptr);
        free(client);
        client = NULL;
        free(ev->data.ptr);
        ev->data.ptr = NULL;

        return;
      }
    }
    client->buffer_pos += bytes_written;
  }

  // Stop monitoring for writable events
  ev->events = EPOLLIN | EPOLLET;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client->fd, ev) == -1) {
    perror("Epoll ctl mod failed");
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->fd, NULL) == -1) {
      perror("epoll_ctl: EPOLL_CTL_DEL failed");
    }
    close(client->fd);
    remove_client_from_list(&client_list_head, (node_data_t *)ev->data.ptr);
    free(client);
    client = NULL;
    free(ev->data.ptr);
    ev->data.ptr = NULL;
  }
}

int main() {
  // Set up signal handling
  struct sigaction sa;
  sa.sa_handler = handle_signal;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);

  int server_fd, client_socket, epoll_fd;
  struct sockaddr_in address;
  struct epoll_event ev, events[MAX_EVENTS];
  int addrlen = sizeof(address);

  // Create a socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == 0) {
    perror("Socket failed");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

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

  epoll_fd = epoll_create1(0); // a kernel obj that keeps track of multiple fds
                               // and notifies on events
  if (epoll_fd == -1) {
    perror("Epoll create failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  node_data_t *server_event = malloc(sizeof(node_data_t));
  if (!server_event) {
    perror("malloc failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }
  server_event->node_type = 0;
  server_event->data.server_fd = server_fd;

  ev.events = EPOLLIN; // set flag to listen for read events on server socket
  ev.data.ptr = server_event;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
    perror("Epoll ctl add failed");
    close(server_fd);
    close(epoll_fd);
    free(server_event);
    server_event = NULL;
    exit(EXIT_FAILURE);
  }

  printf("Server is listening on port %d\n", PORT);

  // Event Loop
  while (keep_running) {
    int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS,
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

      node_data_t *event_data = (node_data_t *)events[i].data.ptr;

      if (event_data->node_type == 0) {
        // Accept a new client connection
        client_socket = accept(server_fd, (struct sockaddr *)&address,
                               (socklen_t *)&addrlen);
        if (client_socket < 0) {
          perror("Accept failed");
          continue;
        }
        printf("Accepted client socket\n");

        set_non_blocking(client_socket);

        client_t *client = malloc(sizeof(client_t));
        if (!client) {
          perror("malloc failed");
          close(client_socket);
          continue;
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
          continue;
        }
        client_event->node_type = 1;
        client_event->data.client = client;

        printf("Accepted client socket 2\n");

        // Add the client socket to epoll
        ev.events = EPOLLIN | EPOLLET;
        ev.data.ptr = client_event;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &ev) == -1) {
          perror("Epoll ctl add failed");
          close(client_socket);
          free(client);
          client = NULL;
          free(client_event);
          client_event = NULL;
          continue;
        }

        printf("Accepted client socket 3\n");

        // Add client to the linked list for cleanup on shutdown
        add_client_to_list(&client_list_head, client_event);
        printf("added client %p\n", client);
      } else {
        // Handle client disconnect or error
        client_t *client = event_data->data.client;

        if (events[i].events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP)) {
          printf("Disconnect event for %p\n", event_data->data.client);
          // Remove the FD from epoll
          if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->fd, NULL) == -1) {
            perror("epoll_ctl: EPOLL_CTL_DEL failed");
          }
          close(client->fd);
          free(client);
          client = NULL;
          remove_client_from_list(&client_list_head, event_data);
          free(event_data);
          event_data = NULL;
          continue;
        }

        // Handle the client request
        if (events[i].events & EPOLLIN) {
          printf("Ready to read\n");
          handle_client_read(client, &events[i], epoll_fd);
        }
        if (events[i].events & EPOLLOUT) {
          printf("Ready to write\n");
          handle_client_write(client, &events[i], epoll_fd);
        }
      }
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
