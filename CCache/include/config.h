#ifndef CONFIG_H
#define CONFIG_H

#define PORT 8080
#define MAX_EVENTS 64

#ifdef TESTING
//#warning "TESTING macro is defined"
#define BUFFER_SIZE 28
#else
#define BUFFER_SIZE 1024
#endif

#endif // CONFIG_H
