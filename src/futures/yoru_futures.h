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
    int err;
} FutureThreadContext_t;

typedef struct Future_t
{
    pthread_t *thread;
    FutureThreadContext_t *ctx;
} Future_t;

YORU_API void Future_init(Future_t *future, void *(*callback)(void *), void *args);
YORU_API void *Future_await(Future_t *future);
YORU_API void Future_cancel(Future_t *future);
YORU_API void Future_destroy(Future_t *future);
YORU_PRIVATE void *Future_thread_wrapper(void *context);

YORU_API void Future_init(Future_t *future, void *(*callback)(void *), void *args)
{
    pthread_t *thread = (pthread_t *)malloc(sizeof(pthread_t));
    FutureThreadContext_t *ctx = (FutureThreadContext_t *)malloc(sizeof(FutureThreadContext_t));
    ctx->args = args;
    ctx->callback = callback;
    ctx->ready = false;
    future->ctx = ctx;

    ctx->err = pthread_create(thread, NULL, Future_thread_wrapper, (void *)ctx);
    if (ctx->err != 0)
    {
        future->thread = NULL;
        future->ctx->ready = true;
        future->ctx->result = NULL;
    }
    else
    {
        future->thread = thread;
    }
}

YORU_API void *Future_await(Future_t *future)
{
    if (future->thread == NULL)
    {
        return NULL;
    }

    (void)pthread_join(*(future->thread), NULL);
    free(future->thread);
    future->thread = NULL;
    future->ctx->ready = true;
    return future->ctx->result;
}

YORU_API void Future_cancel(Future_t *future)
{
    if (future->thread != NULL)
    {
        pthread_cancel(*(future->thread));
        free(future->thread);
        future->thread = NULL;
    }

    future->ctx->ready = true;
    future->ctx->result = NULL;
}

YORU_API void Future_destroy(Future_t *future)
{
    if (future->thread != NULL)
    {
        free(future->thread);
        future->thread = NULL;
    }

    if (future->ctx != NULL)
    {
        free(future->ctx);
        future->ctx = NULL;
    }
}

YORU_PRIVATE void *Future_thread_wrapper(void *context)
{
    FutureThreadContext_t *ctx = (FutureThreadContext_t *)context;
    ctx->result = ctx->callback(ctx->args);
    ctx->ready = true;
    return ctx->result;
}

#endif
