#include "../yoru.h"

#define VECTOR_SIZE 32

typedef struct Data_t
{
    i32 data;
} Data_t;

int main(void)
{
    // using heap allocator because arena does not support reallocation
    // and we need to reallocate the vector when it grows!!
    // the library will raise an error if you try to use arena allocator and try to reallocate
    Allocator_t *allocator = HeapAllocator_new();
    Vec_t(Data_t) vec = vec_new(Data_t, allocator);

    for (i32 i = 0; i < VECTOR_SIZE; i++)
    {
        Data_t data = {.data = i * 2};
        vec_append(vec, data, allocator);
    }

    for (i32 i = 0; i < VECTOR_SIZE; i++)
    {
        Data_t *data = vec_get(vec, i);
        printf("Data at index %d: %d\n", i, data->data);
    }

    // out of bounds access exits application with an error
    // vec_get(vec, 2 * VECTOR_SIZE);

    vec_destroy(vec, allocator);
    return 0;
}