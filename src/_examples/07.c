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
    struct work_args_t *args = (struct work_args_t *)arg;
    for (size_t i = 0; i < args->iterations; i++)
    {
        printf("[do_work] iteration %zu\n", i);
        Sleep(1000);
    }

    printf("[do_work] work done!\n");
    return "Work completed!";
}

int main(void) 
{
    Yoru_Future_t future = {0};

    struct work_args_t args = { .iterations = 5 };
    Future_init(&future, do_work, (void *)&args);
    printf("[main] waiting for future to complete...\n");

    for (size_t i = 0; i < 10; i++)
    {
        if (!future.ctx->ready)
        {
            printf("[main] future not ready yet, waiting...\n");
            Sleep(1000);
        }
        else
        {
            printf("[main] future is ready!\n");
            break;
        }
    }

    void *result = Future_await(&future);

    if (result != NULL)
    {
        printf("[main] future completed with result: %s\n", (char *)result);
    }
    else
    {
        printf("[main] future did not complete successfully.\n");
    }

    Future_destroy(&future);
    printf("[main] future destroyed.\n");
    return 0;
}