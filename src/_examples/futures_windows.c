#if !defined(_WIN32) && !defined(_WIN64)
#error "This example is designed to run on Windows. Please use a Windows environment."
#endif

#define YORU_IMPLEMENTATION
#include "../yoru.h"
#include "windows.h"

#include <stdio.h>

struct work_args_t
{
    size_t iterations;
};

void *do_work(void *arg)
{
    DWORD thread_id = GetCurrentThreadId();
    struct work_args_t *args = (struct work_args_t *)arg;
    for (size_t i = 0; i < args->iterations; i++)
    {
        printf("[do_work:%lu] iteration %zu\n", thread_id, i);
        Sleep(1000);
    }

    printf("[do_work:%lu] work done!\n", thread_id);
    char *result = "Work completed!";
    return (void *)result;
}

int main(void) 
{
    Yoru_Future_t future = {0};

    struct work_args_t args = { .iterations = 5 };
    DWORD start_time = GetTickCount();
    Yoru_Future_init(&future, do_work, (void *)&args);
    printf("[main] started thread to do work...\n");
    printf("[main] waiting for future to complete...\n");
    
    char *result = (char *)Yoru_Future_await(&future);
    DWORD elapsed = GetTickCount() - start_time;
    printf("[main] future completed in %lu ms\n", elapsed);

    if (result != NULL)
    {
        printf("[main] future completed with result: %s\n", (char *)result);
    }
    else
    {
        printf("[main] future did not complete successfully.\n");
    }

    Yoru_Future_destroy(&future);
    printf("[main] future destroyed.\n");
    return 0;
}