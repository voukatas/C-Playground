#ifndef TTL_H
#define TTL_H

#include <time.h>

typedef struct ttl_entry {
    //char *key;
    void *value;
    time_t timestamp;
    int ttl;
} ttl_entry_t;

#endif // TTL_H
