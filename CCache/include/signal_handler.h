#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <signal.h>
#include <stdlib.h>

void handle_signal();
void setup_signal_handling(void);
extern volatile sig_atomic_t keep_running;

#endif // SIGNAL_HANDLER_H
