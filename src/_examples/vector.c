#define YORU_IMPLEMENTATION
#include "../yoru.h"

#define VECTOR_SIZE 32

typedef struct Data_t
{
    i32 data;
} Data_t;

void unsafe_vector_example();
void safe_vector_example();

int main(void)
{
    printf("Safe vector example:\n");
    safe_vector_example();
    printf("----------------------------------------------\n");
    printf("Unsafe vector example:\n");
    unsafe_vector_example();
    return 0;
}

void safe_vector_example()
{
    Yoru_Allocator_t *allocator = Yoru_HeapAllocator_new();
    Vec_t(Data_t) vec = vec_new(Data_t, allocator);
    Yoru_Result_t res;
    Yoru_Error_t err;

    for (i32 i = 0; i < VECTOR_SIZE; i++)
    {
        Data_t data = {.data = i * 2};
        err = vec_append_try(vec, data, allocator);
        if (err != YORU_OK)
        {
            printf("Error appending data: %s\n", yoru_error_to_string(err));
        }
    }

    for (i32 i = 0; i < VECTOR_SIZE; i++)
    {
        res = vec_get_try(vec, i);
        if (res.err != YORU_OK)
        {
            printf("Error getting data at index %d: %s\n", i, yoru_error_to_string(res.err));
        }
        Data_t *data = (Data_t *)res.value;
        printf("Data at index %d: %d\n", i, data->data);
    }

    // out of bounds access exits application with an error
    res = vec_get_try(vec, 2 * VECTOR_SIZE);
    if (res.err != YORU_OK)
    {
        printf("Error getting data at index %d: %s\n", 2 * VECTOR_SIZE, yoru_error_to_string(res.err));
    }

    // set value out of bounds (vector wont resize)
    Data_t new_data = {.data = 100};
    err = vec_set_try(vec, 2 * VECTOR_SIZE, new_data);
    if (err != YORU_OK)
    {
        printf("Error setting data at index %d: %s\n", 2 * VECTOR_SIZE, yoru_error_to_string(err));
    }

    vec_destroy(vec, allocator);
}

void unsafe_vector_example()
{
    // using heap allocator because arena does not support reallocation
    // and we need to reallocate the vector when it grows!!
    // the library will raise an error if you try to use arena allocator and try to reallocate
    Yoru_Allocator_t *allocator = Yoru_HeapAllocator_new();
    Vec_t(Data_t) vec = vec_new(Data_t, allocator);

    for (i32 i = 0; i < VECTOR_SIZE; i++)
    {
        Data_t data = {.data = i * 2};
        vec_append(vec, data, allocator);
    }

    for (i32 i = 0; i < VECTOR_SIZE; i++)
    {
        Data_t *data = vec_get(Data_t *, vec, i);
        printf("Data at index %d: %d\n", i, data->data);
    }

    // out of bounds access exits application with an error
    // vec_get(vec, 2 * VECTOR_SIZE);

    // set value at index 13, if the index is out of bounds, the vector DOES NOT resize
    Data_t new_data = {.data = 100};
    vec_set(vec, 13, new_data);
    Data_t *data = vec_get(Data_t *, vec, 13);
    printf("Data at index 13 after set: %d\n", data->data);

    vec_destroy(vec, allocator);
}