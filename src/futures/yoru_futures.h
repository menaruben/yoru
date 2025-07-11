#ifndef __YORU_FUTURES_H__
#define __YORU_FUTURES_H__

#ifdef _POSIX_VERSION
#include <pthread.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include "../yoru_defs.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct Yoru_ThreadContext_t
{
    void *(*callback)(void *);
    void *args;
    void *result;
    bool ready;
    int err;
} Yoru_ThreadContext_t;

typedef struct Yoru_Future_t
{
#ifdef _POSIX_VERSION
    pthread_t thread;
#elif defined(_WIN32) || defined(_WIN64)
    HANDLE thread;
#else
    #error "Unsupported platform for threads. Please use POSIX or Windows."
#endif
    Yoru_ThreadContext_t *ctx;
} Yoru_Future_t;

YORU_API void Future_init(Yoru_Future_t *future, void *(*callback)(void *), void *args);
YORU_API void *Future_await(Yoru_Future_t *future);
YORU_API void Future_cancel(Yoru_Future_t *future);
YORU_API void Future_destroy(Yoru_Future_t *future);
YORU_PRIVATE void *Future_thread_wrapper(void *context);

#ifdef YORU_IMPLEMENTATION
#ifdef _POSIX_VERSION
YORU_API void Future_init(Yoru_Future_t *future, void *(*callback)(void *), void *args)
{
    pthread_t *thread = (pthread_t *)malloc(sizeof(pthread_t));
    if (!thread)
    {
        YORU_ERROR("Failed to allocate memory for thread.");
    }

    Yoru_ThreadContext_t *ctx = (Yoru_ThreadContext_t *)malloc(sizeof(Yoru_ThreadContext_t));
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

YORU_API void *Future_await(Yoru_Future_t *future)
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

YORU_API void Future_cancel(Yoru_Future_t *future)
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

YORU_API void Future_destroy(Yoru_Future_t *future)
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
    Yoru_ThreadContext_t *ctx = (Yoru_ThreadContext_t *)context;
    ctx->result = ctx->callback(ctx->args);
    ctx->ready = true;
    return ctx->result;
}
#elif defined(_WIN32) || defined(_WIN64)

YORU_API void Future_init(Yoru_Future_t *future, void *(*callback)(void *), void *args)
{
    HANDLE *thread = (HANDLE *)malloc(sizeof(HANDLE));
    if (!thread)
    {
        YORU_ERROR("Failed to allocate memory for thread.");
    }

    Yoru_ThreadContext_t *ctx = (Yoru_ThreadContext_t *)malloc(sizeof(Yoru_ThreadContext_t));
    ctx->args = args;
    ctx->callback = callback;
    ctx->ready = false;
    future->ctx = ctx;

    *thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Future_thread_wrapper, (void *)ctx, 0, NULL);
    if (*thread == NULL)
    {
        free(thread);
        future->thread = NULL;
        future->ctx->ready = true;
        future->ctx->result = NULL;
        return;
    }

    future->thread = thread;
}

YORU_API void *Future_await(Yoru_Future_t *future)
{
    if (future->thread == NULL)
    {
        return NULL;
    }

    WaitForSingleObject(future->thread, INFINITE);
    CloseHandle(future->thread);
    free(future->thread);
    future->thread = NULL;
    future->ctx->ready = true;
    return future->ctx->result;
}

YORU_API void Future_cancel(Yoru_Future_t *future)
{
    if (future->thread != NULL)
    {
        TerminateThread(future->thread, 0);
        CloseHandle(future->thread);
        free(future->thread);
        future->thread = NULL;
    }

    future->ctx->ready = true;
    future->ctx->result = NULL;
}

YORU_API void Future_destroy(Yoru_Future_t *future)
{
    if (future->thread != NULL)
    {
        CloseHandle(future->thread);
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
    Yoru_ThreadContext_t *ctx = (Yoru_ThreadContext_t *)context;
    ctx->result = ctx->callback(ctx->args);
    ctx->ready = true;
    return ctx->result;
}

#else
#include "../utils/yoru_utils.h"

YORU_API void Future_init(Yoru_Future_t *future, void *(*callback)(void *), void *args)
{
    YORU_NOT_SUPPORTED("Futures are currenlty only supported on POSIX systems.");
}

YORU_API void *Future_await(Yoru_Future_t *future)
{
    YORU_NOT_SUPPORTED("Futures are currenlty only supported on POSIX systems.");
    return NULL;
}

YORU_API void Future_cancel(Yoru_Future_t *future)
{
    YORU_NOT_SUPPORTED("Futures are currenlty only supported on POSIX systems.");
}

YORU_API void Future_destroy(Yoru_Future_t *future)
{
    YORU_NOT_SUPPORTED("Futures are currenlty only supported on POSIX systems.");
}

YORU_PRIVATE void *Future_thread_wrapper(void *context)
{
    YORU_NOT_SUPPORTED("Futures are currenlty only supported on POSIX systems.");
    return NULL;
}

#endif
#endif // YORU_IMPLEMENTATION

#endif
