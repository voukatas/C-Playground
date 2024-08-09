#include "../include/signal_handler.h"

volatile sig_atomic_t keep_running = 1;

void handle_signal() {
        keep_running = 0;
}

void setup_signal_handling(void) {
        struct sigaction sa;
        sa.sa_handler = handle_signal;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;

        sigaction(SIGINT, &sa, NULL);
        sigaction(SIGTERM, &sa, NULL);
}
