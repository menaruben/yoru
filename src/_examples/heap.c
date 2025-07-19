#define YORU_IMPLEMENTATION
#include "../yoru.h"
#include <stdio.h>

#define ARRAY_SIZE 10

int main(void)
{
    Yoru_Slice_t arr1 = {0};
    Yoru_Error_t err = yoru_heap_alloc(sizeof(int) * ARRAY_SIZE, &arr1);
    if (err.type != YORU_OK)
    {
        fprintf(stderr, "Failed to allocate memory: %s\n", yoru_error_to_string(err.type));
        if (err.message)
        {
            fprintf(stderr, "Error message: %s\n", err.message);
        }
        return 1;
    }

    printf("Allocated memory for %zu integers.\n", arr1.capacity / sizeof(int));
    printf("Memory address: %p\n", arr1.data);
    printf("Offset: %zu, Capacity: %zu\n", arr1.offset, arr1.capacity);

    for (size_t i = 0; i < ARRAY_SIZE; ++i)
    {
        ((int *)arr1.data)[i] = (int)i;
    }

    printf("Array contents:\n");
    for (size_t i = 0; i < ARRAY_SIZE; ++i)
    {
        printf("%d ", ((int *)arr1.data)[i]);
    }
    printf("\n");
    return 0;
}