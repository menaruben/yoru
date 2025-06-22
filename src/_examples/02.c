#include "../yoru.h"
#include <stdio.h>

List_T(i32);

int main()
{
    ArenaAllocator_t *arena = ArenaAllocator_Init(1000 * sizeof(i32));
    List_i32 list = List_i32_Init();

    for (i32 i = 0; i < 5; i++)
    {
        List_i32_PushBack(arena, &list, i * 10);
    }
    // [0, 10, 20, 30, 40]

    List_i32_Insert(arena, &list, 2, 999); // [0, 10, 999, 20, 30, 40]
    List_i32_Remove(&list, 3);              // [0, 10, 999, 30, 40]

    for (i32 i = 0; i < list.size; i++)
    {
        i32 *val = List_i32_Get(&list, i);
        printf("List[%d] = %d\n", i, *val);
    }

    ArenaAllocator_Free(arena);
    return 0;
}
