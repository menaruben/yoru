#ifndef __YORU_FUTURES_H__
#define __YORU_FUTURES_H__

#include "../yoru_platforms.h"

#if YORU_PLATFORM_UNIX
#include <pthread.h>
#elif YORU_PLATFORM_WINDOWS
#include <windows.h>
#else
#error "Unsupported platform for threads. Please use UNIX/POSIX or Windows."
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
#if YORU_PLATFORM_UNIX
    pthread_t thread;
#elif YORU_PLATFORM_WINDOWS
    HANDLE thread;
#else
#error "Unsupported platform for threads. Please use POSIX or Windows."
#endif
    Yoru_ThreadContext_t *ctx;
} Yoru_Future_t;

YORU_API void Yoru_Future_init(Yoru_Future_t *future, void *(*callback)(void *), void *args);
YORU_API void *Yoru_Future_await(Yoru_Future_t *future);
YORU_API void Yoru_Future_cancel(Yoru_Future_t *future);
YORU_API void Yoru_Future_destroy(Yoru_Future_t *future);
YORU_PRIVATE void *Yoru_Future_thread_wrapper(void *context);

#ifdef YORU_IMPLEMENTATION
#if YORU_PLATFORM_UNIX
YORU_API void Yoru_Future_init(Yoru_Future_t *future, void *(*callback)(void *), void *args)
{
    Yoru_ThreadContext_t *ctx = (Yoru_ThreadContext_t *)malloc(sizeof(Yoru_ThreadContext_t));
    ctx->args = args;
    ctx->callback = callback;
    ctx->ready = false;
    future->ctx = ctx;

    ctx->err = pthread_create(&future->thread, NULL, Yoru_Future_thread_wrapper, (void *)ctx);
    if (ctx->err != 0)
    {
        future->thread = 0;
        future->ctx->ready = true;
        future->ctx->result = NULL;
    }
}

YORU_API void *Yoru_Future_await(Yoru_Future_t *future)
{
    if (future->thread == 0)
    {
        return NULL;
    }

    (void)pthread_join(future->thread, NULL);
    future->thread = 0;
    future->ctx->ready = true;
    return future->ctx->result;
}

YORU_API void Yoru_Future_cancel(Yoru_Future_t *future)
{
    if (future->thread != 0)
    {
        pthread_cancel(future->thread);
        future->thread = 0;
    }

    future->ctx->ready = true;
    future->ctx->result = NULL;
}

YORU_API void Yoru_Future_destroy(Yoru_Future_t *future)
{
    future->thread = 0;

    if (future->ctx != NULL)
    {
        free(future->ctx);
        future->ctx = NULL;
    }
}

YORU_PRIVATE void *Yoru_Future_thread_wrapper(void *context)
{
    Yoru_ThreadContext_t *ctx = (Yoru_ThreadContext_t *)context;
    ctx->result = ctx->callback(ctx->args);
    ctx->ready = true;
    return ctx->result;
}
#elif YORU_PLATFORM_WINDOWS
YORU_API void Yoru_Future_init(Yoru_Future_t *future, void *(*callback)(void *), void *args)
{
    Yoru_ThreadContext_t *ctx = (Yoru_ThreadContext_t *)malloc(sizeof(Yoru_ThreadContext_t));
    ctx->args = args;
    ctx->callback = callback;
    ctx->ready = false;
    future->ctx = ctx;

    future->thread = CreateThread(
        NULL, 0, (LPTHREAD_START_ROUTINE)Yoru_Future_thread_wrapper, (void *)ctx, 0, NULL);
    if (future->thread == NULL)
    {
        future->ctx->ready = true;
        future->ctx->result = NULL;
    }
}

YORU_API void *Yoru_Future_await(Yoru_Future_t *future)
{
    if (future->thread == NULL)
    {
        return NULL;
    }

    WaitForSingleObject(future->thread, INFINITE);
    CloseHandle(future->thread);
    future->thread = NULL;
    future->ctx->ready = true;
    return future->ctx->result;
}

YORU_API void Yoru_Future_cancel(Yoru_Future_t *future)
{
    if (future->thread != NULL)
    {
        TerminateThread(future->thread, 0);
        CloseHandle(future->thread);
        future->thread = NULL;
    }

    future->ctx->ready = true;
    future->ctx->result = NULL;
}

YORU_API void Yoru_Future_destroy(Yoru_Future_t *future)
{
    if (future->thread != NULL)
    {
        CloseHandle(future->thread);
        future->thread = NULL;
    }

    if (future->ctx != NULL)
    {
        free(future->ctx);
        future->ctx = NULL;
    }
}

YORU_PRIVATE void *Yoru_Future_thread_wrapper(void *context)
{
    Yoru_ThreadContext_t *ctx = (Yoru_ThreadContext_t *)context;
    ctx->result = ctx->callback(ctx->args);
    ctx->ready = true;
    return ctx->result;
}

#else

#include "../utils/yoru_utils.h"
#define ERROR_MSG "Futures are currently only supported on POSIX and Windows systems."

YORU_API void Yoru_Future_init(Yoru_Future_t *future, void *(*callback)(void *), void *args)
{
    YORU_NOT_SUPPORTED(ERROR_MSG);
}

YORU_API void *Yoru_Future_await(Yoru_Future_t *future)
{
    YORU_NOT_SUPPORTED(ERROR_MSG);
    return NULL;
}

YORU_API void Yoru_Future_cancel(Yoru_Future_t *future)
{
    YORU_NOT_SUPPORTED(ERROR_MSG);
}

YORU_API void Yoru_Future_destroy(Yoru_Future_t *future)
{
    YORU_NOT_SUPPORTED(ERROR_MSG);
}

YORU_PRIVATE void *Yoru_Future_thread_wrapper(void *context)
{
    YORU_NOT_SUPPORTED(ERROR_MSG);
    return NULL;
}
#endif // else

#endif // YORU_IMPLEMENTATION

#endif
