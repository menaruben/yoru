#include "../yoru.h"

DynArray_T(i32);

int main(void)
{
    DynArray_i32 arr = DynArray_i32_Init();
    for (i32 i = 0; i < 1024; i++)
    {
        DynArray_i32_Put(&arr, i, i * 2);
    }

    for (i32 i = 0; i < arr.size; i++)
    {
        i32 *val = DynArray_i32_Get(&arr, i);
        printf("DynArray[%d] = %d\n", i, *val);
    }

    DynArray_i32_Free(&arr);
    return 0;
}