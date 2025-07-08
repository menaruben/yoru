#ifndef __YORU_FUTURES_H__
#define __YORU_FUTURES_H__

#include <pthread.h>
#include "../yoru_defs.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct FutureThreadContext_t
{
    void *(*callback)(void *);
    void *args;
    void *result;
    bool ready;
} FutureThreadContext_t;

typedef struct Future_t
{
    pthread_t *thread;
    FutureThreadContext_t *ctx;
} Future_t;

YORU_API void Future_init(Future_t *future, void *(*callback)(void *), void *args);
YORU_API void *Future_await(Future_t *future);
YORU_PRIVATE void *Future_thread_wrapper(void *context);

YORU_API void Future_init(Future_t *future, void *(*callback)(void *), void *args)
{
    pthread_t *thread = (pthread_t *)malloc(sizeof(pthread_t));

    FutureThreadContext_t *ctx = malloc(sizeof(FutureThreadContext_t));
    ctx->args = args;
    ctx->callback = callback;
    ctx->ready = false;

    pthread_create(thread, NULL, Future_thread_wrapper, (void *)ctx);
    future->thread = thread;
    future->ctx = ctx;
}

YORU_API void *Future_await(Future_t *future)
{
    (void)pthread_join(*(future->thread), NULL);
    future->ctx->ready = true;
    return future->ctx->result;
}

YORU_PRIVATE void *Future_thread_wrapper(void *context)
{
    FutureThreadContext_t *ctx = (FutureThreadContext_t *)context;
    ctx->result = ctx->callback(ctx->args);
    ctx->ready = true;
    return ctx->result;
}

#endif