#define TESTING
// #include "../include/hashtable.h"
// #include "../include/mock_malloc.h"
#include "../include/client.h"
#include "../include/server.h"
#include "../include/signal_handler.h"
#include "../unity/unity.h"
#include <arpa/inet.h>
#include <pthread.h>

void setUp(void) {
}

void tearDown(void) {
}

// Test cases

void test_add_and_clean_client(void) {
        client_node_t *head = NULL;
        node_data_t *client_data = malloc(sizeof(node_data_t));

        int client_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (client_fd == 0) {
                perror("Socket failed");
                exit(EXIT_FAILURE);
        }
        client_t *client = malloc(sizeof(client_t));
        if (!client) {
                perror("malloc failed");
                close(client_fd);
                return;
        }
        // printf("Accepted client socket 1\n");

        client->fd = client_fd;
        client_data->data.client = client;
        client->read_buffer_len = 0;
        client->write_buffer_len = 0;
        client->write_buffer_pos = 0;
        // close(current->client_data->data.client->fd);

        add_client_to_list(&head, client_data);
        TEST_ASSERT_NOT_NULL(head);
        TEST_ASSERT_EQUAL_PTR(head->client_data, client_data);

        cleanup_all_clients(&head);
}

void test_add_and_remove_client(void) {
        client_node_t *head = NULL;
        node_data_t *client_data = malloc(sizeof(node_data_t));

        int client_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (client_fd == 0) {
                perror("Socket failed");
                exit(EXIT_FAILURE);
        }
        client_t *client = malloc(sizeof(client_t));
        if (!client) {
                perror("malloc failed");
                close(client_fd);
                return;
        }
        // printf("Accepted client socket 1\n");

        client->fd = client_fd;
        client_data->data.client = client;
        client->read_buffer_len = 0;
        client->write_buffer_len = 0;
        client->write_buffer_pos = 0;
        // close(current->client_data->data.client->fd);

        add_client_to_list(&head, client_data);
        TEST_ASSERT_NOT_NULL(head);
        TEST_ASSERT_EQUAL_PTR(head->client_data, client_data);

        remove_client_from_list(&head, client_data);
        TEST_ASSERT_NULL(head);
}

void *run_server_thread(void *arg) {
        int *port = (int *)arg;
        run_server(*port);
        // run_server(*port, 3, 1);
        return NULL;
}

int connect_client(int port, char *ip) {

        int sockfd;
        struct sockaddr_in server_addr;
        // char buffer[BUFFER_SIZE];

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
                perror("Socket creation failed");
                exit(EXIT_FAILURE);
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
                perror("Invalid address");
                close(sockfd);
                exit(EXIT_FAILURE);
        }

        if (connect(sockfd, (struct sockaddr *)&server_addr,
                    sizeof(server_addr)) < 0) {
                perror("Connection to the server failed");
                close(sockfd);
                exit(EXIT_FAILURE);
        }

        return sockfd;
}

void disconnect_client(int sockfd) {

        close(sockfd);
}

void connect_disconnect_client(int port, char *ip) {
        int sockfd = connect_client(port, ip);
        // connect_client(port, ip);
        disconnect_client(sockfd);
}

void send_client_msg(char *msg, int port, char *ip) {

        char buffer[BUFFER_SIZE];
        int sockfd = connect_client(port, ip);

        if (send(sockfd, msg, strlen(msg), 0) < 0) {
                perror("Send failed");
                close(sockfd);
                exit(EXIT_FAILURE);
        }

        // sleep(10);
        // if (send(sockfd, "Another message\r\n", strlen(msg), 0) < 0) {
        //         perror("Send failed");
        //         close(sockfd);
        //         exit(EXIT_FAILURE);
        // }

        int bytes_received = read(sockfd, buffer, sizeof(buffer));
        if (bytes_received < 0) {
                perror("Receive failed");
                close(sockfd);
                exit(EXIT_FAILURE);
        }
        buffer[bytes_received] = '\0';
        // printf("Msg rcvd: %s", buffer);
        TEST_ASSERT_EQUAL_STRING("Hello World from Server\r\n", buffer);

        disconnect_client(sockfd);
        // close(sockfd);
}

void test_run_server_initialization(void) {
        handle_shutdown_signal(1);
        pthread_t server_thread;
        int port = 8080;

        pthread_create(&server_thread, NULL, run_server_thread, &port);

        // Small delay for server to init
        sleep(1);
        send_client_msg("Hello from client\r\n", port, "127.0.0.1");
        // To shutdown the event loop
        sleep(1);
        handle_shutdown_signal(0);
        connect_disconnect_client(port, "127.0.0.1");

        pthread_join(server_thread, NULL);
}

// Placeholder, finish this test after the text-based protocol is finished. This
// covers the branch of code where a node-client from the middle of the linked
// list is removed
void test_run_server_multiple_clients(void) {
        handle_shutdown_signal(1);
        pthread_t server_thread;
        int port = 8080;
        char *ip = "127.0.0.1";

        pthread_create(&server_thread, NULL, run_server_thread, &port);

        // Small delay for server to init
        sleep(1);

        // Test scenario
        connect_client(port, ip);
        // send_client_msg("Hello from client\n", port, ip);
        int sockfd2 = connect_client(port, ip);
        connect_client(port, ip);
        // Verify that the num of clients is 3 using some text-based command
        /*
         * Write test code here
         * */

        disconnect_client(sockfd2);
        // Verify that the num of clients is 2 using some text-based command
        /*
         * Write test code here
         * */

        sleep(1);
        //  To shutdown the event loop
        handle_shutdown_signal(0);
        connect_disconnect_client(port, "127.0.0.1");

        pthread_join(server_thread, NULL);
}
// To-Do
void test_handle_client_read_and_write(void) {
}

int main(void) {
        UNITY_BEGIN();
        RUN_TEST(test_add_and_remove_client);
        RUN_TEST(test_add_and_clean_client);
        RUN_TEST(test_run_server_initialization);
        RUN_TEST(test_run_server_multiple_clients);

        //  RUN_TEST(test_handle_client_read_and_write);
        return UNITY_END();
}
