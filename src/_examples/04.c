#include "../yoru.h"

#define LIST_SIZE 10

int main(void)
{
    Yoru_Allocator_t *allocator = Yoru_HeapAllocator_new();
    YORU_ASSERT_NOT_NULL(allocator);

    List_t(i32) list = list_new(i32, allocator);
    YORU_ASSERT_NOT_NULL(list.head);

    // Use a static array to store values
    i32 values[LIST_SIZE];
    for (i32 i = 0; i < LIST_SIZE; i++)
    {
        values[i] = i;
        list_append(allocator, list, &values[i]);
    }

    for (i32 i = 0; i < LIST_SIZE; i++)
    {
        i32 *data = list_get(list, i);
        YORU_ASSERT_NOT_NULL(data);
        printf("Data at index %d: %d\n", i, *data);
    }

    i32 new_value = 100;
    list_prepend(allocator, list, &new_value);
    i32 *first_data = list_get(list, 0);
    YORU_ASSERT_NOT_NULL(first_data);
    ASSERT_EQUAL(*first_data, new_value);

    list_remove(allocator, list, 4);
    i32 *new_data = list_get(list, 4);
    YORU_ASSERT_NOT_NULL(new_data);
    ASSERT_EQUAL(*new_data, 4);
    // prepended 100 so 4 should be at index 5 but after
    // removing index data at index 4, 4 should be at index 4

    list_insert(allocator, list, 2, &new_value);
    i32 *inserted_data = list_get(list, 2);
    YORU_ASSERT_NOT_NULL(inserted_data);
    ASSERT_EQUAL(*inserted_data, new_value);

    // out of bounds access exits application with an error
    // i32 *out_of_bounds_data = list_get(list, list.size + 1);

    list_destroy(list, allocator);
    return 0;
}