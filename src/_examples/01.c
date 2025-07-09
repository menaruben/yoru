#include "../yoru.h"

#define ARRAY_SIZE 10

#define ARENA_SIZE (1024 * sizeof(int))

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
    Yoru_Allocator_t *allocator = Yoru_ArenaAllocator_new(ARENA_SIZE);
    YORU_ASSERT_NOT_NULL(allocator);

    // allocate an array on arena
    int *arr1 = (int *)allocator->alloc(allocator->context, sizeof(int) * ARRAY_SIZE);
    YORU_ASSERT_NOT_NULL(arr1);
    INIT_ARRAY(arr1);
    PRINT_ARRAY(arr1);

    // allocate another array on arena
    int *arr2 = (int *)allocator->alloc(allocator->context, sizeof(int) * ARRAY_SIZE);
    YORU_ASSERT_NOT_NULL(arr2);
    INIT_ARRAY(arr2);
    PRINT_ARRAY(arr2);

    // allocate array too large for the arena
    int *arr3 = (int *)allocator->alloc(allocator->context, 2 * ARENA_SIZE);
    ASSERT_NULL(arr3); // should return NULL since the arena is too

    // instead of freeing each array, we free the entire arena instead :)
    allocator->free(allocator->context, NULL); // free arena
    return 0;
}