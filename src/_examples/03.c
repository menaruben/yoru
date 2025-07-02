#include "../yoru.h"

#define VECTOR_SIZE 1024

int main(void)
{
    Allocator_t *allocator = HeapAllocator_new();
    Vec_t(i32) vec = vec_new(i32, allocator);

    for (i32 i = 0; i < VECTOR_SIZE; i++)
    {
        vec_push(vec, i, allocator);
    }

    for (i32 i = 0; i < VECTOR_SIZE; i++)
    {
        i32 *value = vec_get(vec, i);
        printf("vec[%d] = %d\n", i, *value);
    }

    vec_destroy(vec, allocator);
    return 0;
}