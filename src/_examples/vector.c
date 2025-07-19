#define YORU_IMPLEMENTATION
#include "../yoru.h"

int main(void)
{
    Yoru_Vector_t vector;
    Yoru_Error_t err = yoru_vector_init(sizeof(i32), &vector);
    if (err.type != YORU_OK)
    {
        printf("Error initializing vector: %s\n", err.message);
        return 1;
    }
    printf("Vector initialized with item size: %zu, initial size: %zu\n", vector.vector.item_size, vector.vector.size);
    printf("Vector slice capacity [bytes]: %zu\n", vector.vector.slice.capacity);
    printf("Vector slice offset [bytes]: %zu\n", vector.vector.slice.offset);
    printf("Vector slice data pointer: from %p to %p\n", vector.vector.slice.data, (char *)vector.vector.slice.data + vector.vector.slice.capacity);

    for (size_t i = 0; i < 64; ++i)
    {
        i32 value = (i32)(i * 5);
        err = yoru_vector_append(&vector, &value);
        if (err.type != YORU_OK)
        {
            printf("Error appending value %d: %s\n", value, err.message);
            return 1;
        }
        printf("Appended value: %d\n", value);
    }

    printf("Vector size after appending: %zu\n", yoru_vector_size(&vector));

    for (size_t i = 0; i < yoru_vector_size(&vector); ++i)
    {
        i32 value = 0;
        err = yoru_vector_get(&vector, i, &value);
        if (err.type != YORU_OK)
        {
            printf("Error getting value at index %zu: %s\n", i, err.message);
            return 1;
        }
        printf("vector[%zu] = %d\n", i, value);
    }

    yoru_vector_free(&vector);
    return 0;
}