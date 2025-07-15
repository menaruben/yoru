#define YORU_IMPLEMENTATION
#include "../yoru.h"
#include <stdio.h>

int main(void)
{
    Yoru_Allocator_Result_t arena_result = yoru_arena_alloc(1024 * sizeof(int));
    if (arena_result.err != YORU_OK)
    {
        fprintf(stderr, "Failed to allocate arena: %s\n", yoru_error_to_string(arena_result.err));
        return 1;
    }

    Yoru_Slice_t arena = arena_result.value;
    Yoru_Allocator_Result_t slice_result = yoru_arena_alloc_slice(&arena, 256 * sizeof(int));
    if (slice_result.err != YORU_OK)
    {
        fprintf(stderr, "Failed to allocate slice: %s\n", yoru_error_to_string(slice_result.err));
        yoru_arena_free(&arena);
        return 1;
    }

    Yoru_Slice_t slice = slice_result.value;
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

    yoru_arena_free(&arena);
    return 0;
}