// Example usage of Yoru_List_t
#include <stdio.h>
#define YORU_IMPLEMENTATION
#include "../yoru.h"

int main(void)
{
    Yoru_List_t list;
    int value;

    // Initialize the list
    yoru_list_init(&list);

    // Append integers to the list
    int a = 10, b = 20, c = 30;
    yoru_list_append(&list, &a);
    yoru_list_append(&list, &b);
    yoru_list_prepend(&list, &c); // c will be at index 0

    // Get and print values
    for (size_t i = 0; i < list.size; ++i)
    {
        yoru_list_get(&list, i, &value);
        printf("list[%zu] = %d\n", i, value);
    }

    // Set value at index 1
    int new_val = 99;
    yoru_list_set(&list, 1, &new_val);

    // Print updated list
    printf("After set:\n");
    for (size_t i = 0; i < list.size; ++i)
    {
        yoru_list_get(&list, i, &value);
        printf("list[%zu] = %d\n", i, value);
    }

    // Remove index 0
    yoru_list_remove(&list, 0);

    // Print after removal
    printf("After remove:\n");
    for (size_t i = 0; i < list.size; ++i)
    {
        yoru_list_get(&list, i, &value);
        printf("list[%zu] = %d\n", i, value);
    }

    // Free the list
    yoru_list_free(&list);

    return 0;
}