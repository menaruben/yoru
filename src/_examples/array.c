#define YORU_IMPLEMENTATION
#include "../yoru.h"
#include <string.h>

int main(void)
{
    Yoru_Array_t array = {0};
    Yoru_Error_t err = yoru_array_init(sizeof(i32), 16, &array);
    if (err.type != YORU_OK)
    {
        printf("Error creating array: %s\n (%s)", err.message, yoru_error_to_string(err.type));
        return 1;
    }

    printf("Array initialized with item size: %zu, initial size: %zu\n", array.item_size, array.size);
    printf("Array slice capacity [bytes]: %zu\n", array.slice.capacity);
    printf("Array slice offset [bytes]: %zu\n", array.slice.offset);
    printf("Array slice data pointer: from %p to %p\n", array.slice.data, (char *)array.slice.data + array.slice.capacity);

    for (size_t i = 0; i < array.size; ++i)
    {
        i32 value = (i32)(i * 10);
        err = yoru_array_set(&array, i, &value);
        if (err.type != YORU_OK)
        {
            printf("Error setting value at index %zu: %s\n (%s)", i, err.message, yoru_error_to_string(err.type));
            return 1;
        }
        printf("Set value at index %zu: %d\n", i, value);
    }

    for (size_t i = 0; i < array.size; ++i)
    {
        i32 value = 0;
        err = yoru_array_get(&array, i, &value);
        if (err.type != YORU_OK)
        {
            printf("Error getting value at index %zu: %s\n (%s)", i, err.message, yoru_error_to_string(err.type));
            return 1;
        }
        printf("array[%zu] = %d\n", i, value);
    }

    // out of bounds access, strict
    i32 out_of_bounds_value = 0;
    size_t out_of_bounds_index = 2 * array.size + 5;
    err = yoru_array_get(&array, out_of_bounds_index, &out_of_bounds_value);
    if (err.type != YORU_OK)
    {
        printf("Error for out of bounds access at index %zu: %s (%s)\n", out_of_bounds_index, err.message, yoru_error_to_string(err.type));
    }
    else
    {
        printf("Unexpectedly got value at out of bounds index %zu: %d\n", out_of_bounds_index, out_of_bounds_value);
    }

    array.index_strategy = YORU_ARRAY_INDEX_WRAP_AROUND;
    err = yoru_array_get(&array, out_of_bounds_index, &out_of_bounds_value);
    if (err.type != YORU_OK)
    {
        printf("Error for out of bounds access with wrap around at index %zu: %s (%s)\n", out_of_bounds_index, err.message, yoru_error_to_string(err.type));
    }
    else
    {
        printf("Got value at out of bounds index %zu with wrap around: %d\n", out_of_bounds_index, out_of_bounds_value);
    }
    array.index_strategy = YORU_ARRAY_INDEX_STRICT;

    // copy array to bigger array
    Yoru_Array_t copy_array = {0};
    err = yoru_array_init(sizeof(i32), 45, &copy_array);
    if (err.type != YORU_OK)
    {
        printf("Error creating copy array: %s\n (%s)", err.message, yoru_error_to_string(err.type));
        yoru_array_free(&array);
        return 1;
    }

    err = yoru_array_copy(&array, &copy_array);
    if (err.type != YORU_OK)
    {
        printf("Error copying array: %s\n (%s)", err.message, yoru_error_to_string(err.type));
        yoru_array_free(&array);
        yoru_array_free(&copy_array);
        return 1;
    }
    yoru_array_free(&array); // free original array since we only use new copy_array now

    for (size_t i = 0; i < copy_array.size; ++i)
    {
        i32 value = 0;
        err = yoru_array_get(&copy_array, i, &value);
        if (err.type != YORU_OK)
        {
            printf("Error getting value from copy array at index %zu: %s\n (%s)", i, err.message, yoru_error_to_string(err.type));
            yoru_array_free(&copy_array);
            return 1;
        }
        printf("copy_array[%zu] = %d\n", i, value);
    }

    yoru_array_free(&copy_array);
    printf("Arrays freed successfully.\n");
    return 0;
}
