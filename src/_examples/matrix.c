#define YORU_IMPLEMENTATION
#include "../yoru.h"

#define ROWS 4
#define COLS 4

int main(void)
{
    Yoru_Array_t matrix = {0};
    Yoru_Error_t err = yoru_array_init(sizeof(Yoru_Array_t), ROWS, &matrix);
    if (err.type != YORU_OK)
    {
        printf("Error creating matrix: %s\n (%s)", err.message, yoru_error_to_string(err.type));
        return 1;
    }

    printf("Matrix initialized with item size: %zu, initial size: %zu\n", matrix.item_size, matrix.size);
    printf("Matrix slice capacity [bytes]: %zu\n", matrix.slice.capacity);
    printf("Matrix slice offset [bytes]: %zu\n", matrix.slice.offset);
    printf("Matrix slice data pointer: from %p to %p\n", matrix.slice.data, (char *)matrix.slice.data + matrix.slice.capacity);

    for (size_t i = 0; i < ROWS; ++i)
    {
        Yoru_Array_t row = {0};
        err = yoru_array_init(sizeof(i32), COLS, &row);
        if (err.type != YORU_OK)
        {
            printf("Error creating row %zu: %s\n (%s)", i, err.message, yoru_error_to_string(err.type));
            return 1;
        }

        err = yoru_array_set(&matrix, i, &row);
        if (err.type != YORU_OK)
        {
            printf("Error setting row %zu in matrix: %s\n (%s)", i, err.message, yoru_error_to_string(err.type));
            return 1;
        }
    }

    // pretty print matrix
    printf("Matrix contents:\n");
    for (size_t i = 0; i < ROWS; ++i)
    {
        Yoru_Array_t row = {0};
        err = yoru_array_get(&matrix, i, &row);
        if (err.type != YORU_OK)
        {
            printf("Error getting row %zu from matrix: %s\n (%s)", i, err.message, yoru_error_to_string(err.type));
            return 1;
        }

        for (size_t j = 0; j < COLS; ++j)
        {
            i32 value = (i32)((i + 1) * 10 * COLS + j) % 100;
            err = yoru_array_set(&row, j, &value);
            if (err.type != YORU_OK)
            {
                printf("Error setting value at row %zu, col %zu: %s\n (%s)", i, j, err.message, yoru_error_to_string(err.type));
                return 1;
            }

            printf("%d ", value);
        }
        printf("\n");
    }

    yoru_array_free(&matrix);
    printf("Matrix freed successfully.\n");
}
