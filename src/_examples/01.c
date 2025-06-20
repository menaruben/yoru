#include "../yoru.h"
#include <stdio.h>

// generate type and function defitions for a static array of i32 (typedef for int32_t, see yoru_inttypes.h)
Array_T(i32);

int main()
{
    // create an arena allocator
    ArenaAllocator_t arena = ArenaAllocator_Init(1000 * sizeof(i32));

    // allocate the i32 array of size 10 in the arena
    Array_i32 arr = Array_i32_Init(&arena, 10);

    // fill the array with values
    for (i32 i = 0; i < 20; i++)
    {
        // note: index will get normalized between 0 and length of
        // the array using modulo operation
        // That means if you would do Array_i32_Put(&arr, -1, 20)
        // you would change the last element of the array to 20
        // because -1 % 10 == 9 which is the last index of the array
        Array_i32_Put(&arr, i, i * 2);
    }

    // retrieve and print the values from the array
    for (i32 i = 0; i < 20; i++)
    {
        // note: index will get normalized between 0 and length of
        // the array using modulo operation
        i32 *value = Array_i32_Get(&arr, i);
        if (value)
        {
            printf("Value at index %d: %d\n", i, *value);
        }
        else
        {
            printf("No value at index %d\n", i);
        }
    }

    ArenaAllocator_Free(&arena);
    return 0;
}