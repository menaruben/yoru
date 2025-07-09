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
    Yoru_Allocator_t *allocator = HeapAllocator_new();
    YORU_ASSERT_NOT_NULL(allocator);

    // allocate an array on heap
    int *arr1 = (int *)allocator->alloc(allocator->context, sizeof(int) * ARRAY_SIZE);
    YORU_ASSERT_NOT_NULL(arr1);
    INIT_ARRAY(arr1);
    PRINT_ARRAY(arr1);

    // allocate another array on heap
    int *arr2 = (int *)allocator->alloc(allocator->context, sizeof(int) * ARRAY_SIZE);
    YORU_ASSERT_NOT_NULL(arr2);
    INIT_ARRAY(arr2);
    PRINT_ARRAY(arr2);

    // free the arrays
    allocator->free(allocator->context, arr1);
    allocator->free(allocator->context, arr2);
    return 0;
}