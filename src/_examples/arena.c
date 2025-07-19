#define YORU_IMPLEMENTATION
#include "../yoru.h"
#include <stdio.h>

#define ARENA_SIZE 1024
#define ARRAY_LENGTH 10

int main(void)
{
    Yoru_Slice_t arena = {0};
    Yoru_Error_t err = yoru_arena_alloc(ARENA_SIZE, &arena);
    if (err.type != YORU_OK)
    {
        fprintf(stderr, "Failed to allocate arena: %s\n", yoru_error_to_string(err.type));
        return 1;
    }

    printf("Allocated arena with capacity: %zu bytes\n", arena.capacity);
    printf("Arena address: %p\n", arena.data);
    printf("Offset: %zu, Capacity: %zu\n", arena.offset, arena.capacity);
    printf("Arena is from %p to %p\n", arena.data, (char *)arena.data + arena.capacity);

    Yoru_Slice_t arr = {0};
    err = yoru_arena_alloc_slice(&arena, sizeof(int) * ARRAY_LENGTH, &arr);
    if (err.type != YORU_OK)
    {
        fprintf(stderr, "Failed to allocate arr: %s\n", yoru_error_to_string(err.type));
        yoru_arena_free(&arena);
        return 1;
    }

    printf("Allocated array slice with capacity: %zu bytes\n", arr.capacity);
    printf("Array address: %p\n", arr.data);
    printf("Offset: %zu, Capacity: %zu\n", arr.offset, arr.capacity);
    printf("Array is from %p to %p\n", arr.data, (char *)arr.data + arr.capacity);

    for (size_t i = 0; i < ARRAY_LENGTH; ++i)
    {
        ((int *)arr.data)[i] = (int)i;
    }

    printf("Array contents:\n");
    for (size_t i = 0; i < ARRAY_LENGTH; ++i)
    {
        printf("%d ", ((int *)arr.data)[i]);
    }
    printf("\n");

    yoru_arena_free(&arena);
    return 0;
}