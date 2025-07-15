#define YORU_IMPLEMENTATION
#include "../yoru.h"
#include <stdio.h>

int main(void)
{
    Yoru_Allocator_Result_t res = yoru_heap_alloc(sizeof(int) * 10);
    if (res.err != YORU_OK)
    {
        fprintf(stderr, "Failed to allocate memory: %s\n", yoru_error_to_string(res.err));
        return 1;
    }

    Yoru_Slice_t slice = res.value;
    int *data = (int *)slice.data;
    size_t item_count = slice.capacity / sizeof(int);

    for (size_t i = 0; i < item_count; ++i)
    {
        data[i] = (int)i;
    }

    printf("Allocated slice with %zu items:\n", item_count);
    for (size_t i = 0; i < item_count; ++i)
    {
        printf("%d ", data[i]);
    }
    printf("\n");

    // Free the allocated memory
    if (slice.data)
    {
        yoru_heap_free(&slice);
    }

    return 0;
}