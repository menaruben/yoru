#define YORU_IMPLEMENTATION
#include "../yoru.h"

#define LIST_SIZE 10

void unsafe_list_example(void);
void safe_list_example(void);

int main(int argc, char *argv[])
{
    printf("Safe list example:\n");
    safe_list_example();
    printf("----------------------------------------------\n");
    printf("Unsafe list example:\n");
    unsafe_list_example();
    return 0;
}

void unsafe_list_example(void)
{
    Yoru_Allocator_t *allocator = Yoru_HeapAllocator_new();
    YORU_ASSERT_NOT_NULL(allocator);

    List_t(i32) list = yoru_list_new(i32, allocator);
    YORU_ASSERT_NOT_NULL(list.head);

    // Use a static array to store values
    i32 values[LIST_SIZE];
    for (i32 i = 0; i < LIST_SIZE; i++)
    {
        values[i] = i;
        yoru_list_append(allocator, list, &values[i]);
    }

    for (i32 i = 0; i < LIST_SIZE; i++)
    {
        i32 *data = yoru_list_get(i32 *, list, i);
        YORU_ASSERT_NOT_NULL(data);
        printf("Data at index %d: %d\n", i, *data);
    }

    i32 new_value = 100;
    yoru_list_prepend(allocator, list, &new_value);
    i32 *first_data = yoru_list_get(i32 *, list, 0);
    YORU_ASSERT_NOT_NULL(first_data);
    YORU_ASSERT_EQUAL(*first_data, new_value);

    yoru_list_remove(allocator, list, 4);
    i32 *new_data = yoru_list_get(i32 *, list, 4);
    YORU_ASSERT_NOT_NULL(new_data);
    YORU_ASSERT_EQUAL(*new_data, 4);
    // prepended 100 so 4 should be at index 5 but after
    // removing index data at index 4, 4 should be at index 4

    yoru_list_insert(allocator, list, 2, &new_value);
    i32 *inserted_data = yoru_list_get(i32 *, list, 2);
    YORU_ASSERT_NOT_NULL(inserted_data);
    YORU_ASSERT_EQUAL(*inserted_data, new_value);

    // out of bounds access exits application with an error
    // i32 *out_of_bounds_data = yoru_list_get(list, list.size + 1);

    yoru_list_destroy(list, allocator);
}

void safe_list_example(void)
{
    // smaller example because most of the functions above have a try version
    Yoru_Allocator_t *allocator = Yoru_HeapAllocator_new();
    YORU_ASSERT_NOT_NULL(allocator);

    List_t(i32) list = yoru_list_new(i32, allocator);
    YORU_ASSERT_NOT_NULL(list.head);

    i32 values[LIST_SIZE];
    for (i32 i = 0; i < LIST_SIZE; i++)
    {
        values[i] = i;
        Yoru_Error_t err = yoru_list_append_try(allocator, list, &values[i]);
        if (err != YORU_OK)
        {
            fprintf(stderr, "Error appending value %d: %d\n", values[i], err);
            continue;
        }
    }

    for (i32 i = 0; i < LIST_SIZE; i++)
    {
        Yoru_Result_t result = yoru_list_get_try(i32 *, list, i);
        if (result.err != YORU_OK)
        {
            fprintf(stderr, "Error getting value at index %d: %s\n", i, result.message);
            continue;
        }
        printf("Data at index %d: %d\n", i, *(i32 *)result.value);
    }

    // lets try to give head as null to get a value at index and see what happens
    List_t(i32) x = {0};
    Yoru_Result_t head_null_result = yoru_list_get_try(i32, x, 2);
    printf("error_type: %s\n", yoru_error_to_string(head_null_result.err));

    if (head_null_result.err != YORU_OK)
    {
        if (head_null_result.err == YORU_ERR_ARGUMENT_NULL && head_null_result.message)
        {
            printf("err: %s\n", head_null_result.message);
        }
    }

    // out of bounds error
    Yoru_Result_t out_of_bounds_result = yoru_list_get_try(i32, list, LIST_SIZE * 2);
    printf("error_type: %s\n", yoru_error_to_string(out_of_bounds_result.err));
}