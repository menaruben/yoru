#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <unistd.h>
#include "../yoru.h"

// need to compile with -lcurl !!

#define TIMEOUT_SECONDS 5

#define DEMO_SLEEP_SECONDS 3

struct Get_Response_t
{
    char *content;
    size_t content_length;
};

struct Get_Request_t
{
    const char *url;
    Yoru_Allocator_t *allocator;
};

struct DataWriterContext_t
{
    struct Get_Response_t *response;
    Yoru_Allocator_t *allocator;
};

static size_t write_data(void *contents, size_t size, size_t nmemb, void *userp);
void *http_get(void *request);

int main(void)
{
    Yoru_Allocator_t *allocator = Yoru_HeapAllocator_new();
    YORU_ASSERT_NOT_NULL(allocator);

    struct Get_Request_t req =
        {.url = "https://qrandom.io/api/random/string",
         .allocator = allocator};

    Future_t future = {0};
    Future_init(&future, &http_get, (void *)&req);

    size_t elapsed = 0;
    pthread_t mainthread = pthread_self();
    while (!future.ctx->ready)
    {
        printf("[main: %lu] waiting for future to be fulfilled... (%zu s)\n", mainthread, elapsed);
        sleep(1);
        elapsed += 1;

        if (elapsed >= TIMEOUT_SECONDS)
        {
            printf("[main: %lu] timeout reached, cancelling future...\n", mainthread);
            Future_cancel(&future);
            printf("[main: %lu] future cancelled, exiting...\n", mainthread);
            goto cleanup;
        }
    }

    struct Get_Response_t *res = (struct Get_Response_t *)Future_await(&future);

    if (future.ctx->err)
    {
        printf("[main: %lu] there was an error running the future, error: %d\n", mainthread, future.ctx->err);
        goto cleanup;
    }

    printf("%zu bytes received:\n", res->content_length);
    printf("%s\n", res->content);

cleanup:
    printf("[main: %lu] cleaning up...\n", mainthread);
    Future_destroy(&future);
    return 0;
}

static size_t write_data(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t actual_size = size * nmemb;
    struct DataWriterContext_t *dw_ctx = (struct DataWriterContext_t *)userp;
    struct Get_Response_t *response = dw_ctx->response;
    Yoru_Allocator_t *allocator = dw_ctx->allocator;

    char *new_ptr = (char *)allocator->realloc(response->content, response->content_length + actual_size + 1);
    YORU_ASSERT_NOT_NULL(new_ptr);

    response->content = new_ptr;
    memcpy(&(response->content[response->content_length]), contents, actual_size);
    response->content_length += actual_size;
    response->content[response->content_length] = 0;
    return actual_size;
}

void *http_get(void *req)
{
    pthread_t p = pthread_self();
    printf("[http_get: %lu] http_get running...\n", p);

    struct Get_Request_t *request = (struct Get_Request_t *)req;
    Yoru_Allocator_t *allocator = request->allocator;
    CURL *curl_handle;
    CURLcode res;

    printf("[http_get: %lu] building request...\n", p);
    struct Get_Response_t *response = allocator->alloc(allocator->context, sizeof(struct Get_Response_t));

    response->content = (char *)allocator->alloc(allocator->context, 1);
    YORU_ASSERT_NOT_NULL(response->content);
    response->content[0] = 0;
    response->content_length = 0;

    struct DataWriterContext_t dw_ctx = {response, allocator};

    curl_handle = curl_easy_init();
    YORU_ASSERT_NOT_NULL(curl_handle);

    printf("[http_get: %lu] send get request...\n", p);
    curl_easy_setopt(curl_handle, CURLOPT_URL, request->url);

    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&dw_ctx);

    res = curl_easy_perform(curl_handle);
    const char *err_msg = Yoru_String_to_cstr(Yoru_String_format("curl_easy_perform() failed: %s\n", curl_easy_strerror(res)), allocator);
    YORU_ASSERT(res == CURLE_OK, err_msg);

    printf("[http_get: %lu] clean up...\n", p);
    allocator->free(allocator->context, (void *)err_msg);
    curl_easy_cleanup(curl_handle);

    printf("[http_get: %lu] sleeping...\n", p);
    sleep(DEMO_SLEEP_SECONDS);

    return response;
}