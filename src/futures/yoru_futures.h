#ifndef __YORU_FUTURES_H__
#define __YORU_FUTURES_H__

#include <pthread.h>
#include "../yoru_defs.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct Future_t
{
    bool ready;
    void *args;
    pthread_t *thread;
} Future_t;

YORU_API void Future_init(Future_t *future, void *(*callback)(void *), void *args);
YORU_API void *Future_await(Future_t *future);

YORU_API void Future_init(Future_t *future, void *(*callback)(void *), void *args)
{
    pthread_t *thread = (pthread_t *)malloc(sizeof(pthread_t));
    pthread_create(thread, NULL, callback, args);
    future->thread = thread;
    future->args = args;
    future->ready = false;
}

YORU_API void *Future_await(Future_t *future)
{
    (void)pthread_join(*(future->thread), NULL);
    future->ready = true;
    return future->args;
}

#endif