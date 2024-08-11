#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <signal.h>
#include <stdlib.h>
#include <stdatomic.h>

void handle_shutdown_signal(int state);
void setup_signal_handling(void);
extern atomic_int keep_running;
//extern volatile sig_atomic_t keep_running;

#endif // SIGNAL_HANDLER_H
