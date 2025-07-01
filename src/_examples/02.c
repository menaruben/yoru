#include "../yoru.h"

#define ARRAY_SIZE 10

#define INIT_ARRAY(arr)                         \
    do                                          \
    {                                           \
        for (size_t i = 0; i < ARRAY_SIZE; i++) \
        {                                       \
            arr[i] = (int)i;                    \
        }                                       \
    } while (0)

#define PRINT_ARRAY(arr)                        \
    do                                          \
    {                                           \
        for (size_t i = 0; i < ARRAY_SIZE; i++) \
        {                                       \
            printf("%d ", arr[i]);              \
        }                                       \
        printf("\n");                           \
    } while (0)

int main(void)
{
    Allocator_t *allocator = HeapAllocator_new();
    ASSERT_NOT_NULL(allocator);

    // allocate an array on heap
    int *arr1 = (int *)allocator->alloc(allocator->context, sizeof(int) * ARRAY_SIZE);
    ASSERT_NOT_NULL(arr1);
    INIT_ARRAY(arr1);
    PRINT_ARRAY(arr1);

    // allocate another array on heap
    int *arr2 = (int *)allocator->alloc(allocator->context, sizeof(int) * ARRAY_SIZE);
    ASSERT_NOT_NULL(arr2);
    INIT_ARRAY(arr2);
    PRINT_ARRAY(arr2);

    // free the arrays
    allocator->free(arr1);
    allocator->free(arr2);

    // free the allocator
    free(allocator->context);

    return 0;
}