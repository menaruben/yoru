#ifndef __YORU_VECTOR_H__
#define __YORU_VECTOR_H__

#include "../yoru_defs.h"
#include "yoru_array.h"
#include <stddef.h>

#define YORU_VECTOR_INITIAL_SIZE 16

#define YORU_VECTOR_GROWTH_FACTOR 2

/// @brief Vector is a dynamic array that can grow in size as needed.
typedef struct Yoru_Vector_t
{
    Yoru_Array_t vector;
    size_t highest_set_index; // used for appending
} Yoru_Vector_t;

/// @brief Initialize a vector.
/// @param item_size The size of each item in bytes.
/// @param vector Pointer to the vector to initialize.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the vector is NULL or if memory allocation fails.
YORU_API Yoru_Error_t yoru_vector_init(const size_t item_size, YORU_OUT Yoru_Vector_t *vector);

/// @brief Free the vector.
/// @param vector Pointer to the vector to free.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the vector is NULL.
YORU_API Yoru_Error_t yoru_vector_free(Yoru_Vector_t *vector);

/// @brief Set an item in the vector at a specific index.
/// @param vector Pointer to the vector.
/// @param index The index at which to set the item.
/// @param item Pointer to the item to set in the vector.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the vector is NULL, or if the item is NULL.
YORU_API Yoru_Error_t yoru_vector_set(Yoru_Vector_t *vector, size_t index, const void *item);

/// @brief Get an item from the vector at a specific index.
/// @param vector Pointer to the vector.
/// @param index The index from which to get the item.
/// @param out_item Pointer to the location where the item will be stored.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the vector is NULL, or if the out_item is NULL.
YORU_API Yoru_Error_t yoru_vector_get(const Yoru_Vector_t *vector, size_t index, YORU_OUT void *out_item);

/// @brief Append an item to the vector.
/// @param vector Pointer to the vector.
/// @param item Pointer to the item to append to the vector.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the vector is NULL, or if the item is NULL.
YORU_API Yoru_Error_t yoru_vector_append(Yoru_Vector_t *vector, const void *item);

/// @brief Get the size of the vector.
/// @param vector Pointer to the vector.
/// @return const size_t Returns the number of items in the vector, or 0 if the vector is NULL or has no items.
YORU_API const size_t yoru_vector_size(const Yoru_Vector_t *vector);

#ifdef YORU_IMPLEMENTATION
YORU_API Yoru_Error_t yoru_vector_init(const size_t item_size, YORU_OUT Yoru_Vector_t *vector)
{
    if (!vector)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(vector) " cannot be NULL"};
    }

    Yoru_Error_t err = yoru_array_init(item_size, YORU_VECTOR_INITIAL_SIZE, &vector->vector);
    if (err.type != YORU_OK)
    {
        return err;
    }

    vector->highest_set_index = 0;
    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
}

YORU_API Yoru_Error_t yoru_vector_free(Yoru_Vector_t *vector)
{
    if (!vector)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(vector) " cannot be NULL"};
    }

    return yoru_array_free(&vector->vector);
}

YORU_API Yoru_Error_t yoru_vector_set(Yoru_Vector_t *vector, size_t index, const void *item)
{
    if (!vector)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(vector) " cannot be NULL"};
    }

    if (index > vector->highest_set_index)
    {
        vector->highest_set_index = index;
    }

    return yoru_array_set(&vector->vector, index, item);
}

YORU_API Yoru_Error_t yoru_vector_get(const Yoru_Vector_t *vector, size_t index, YORU_OUT void *out_item)
{
    if (!vector)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(vector) " cannot be NULL"};
    }

    return yoru_array_get(&vector->vector, index, out_item);
}

YORU_API Yoru_Error_t yoru_vector_append(Yoru_Vector_t *vector, const void *item)
{
    if (!vector)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(vector) " cannot be NULL"};
    }

    if (!vector->vector.slice.data)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = "Vector data is NULL"};
    }

    if (vector->highest_set_index + 1 >= vector->vector.size)
    {
        // if target is beyond end of allocated memory of vectors array, we need to copy the array to a larger array with more capacity
        size_t new_capacity = vector->vector.slice.capacity * YORU_VECTOR_GROWTH_FACTOR;
        Yoru_Array_t new_array = {0};
        Yoru_Error_t err = yoru_array_init(vector->vector.item_size, new_capacity, &new_array); // MIGHT LEAD TO UB
        if (err.type != YORU_OK)
        {
            return (Yoru_Error_t){.type = YORU_ERR_ALLOC, .message = "Couldn't allocate memory for the vector slice"};
        }

        err = yoru_array_copy(&vector->vector, &new_array);
        if (err.type != YORU_OK)
        {
            yoru_array_free(&new_array);
            return (Yoru_Error_t){.type = YORU_ERR_ALLOC, .message = "Couldn't copy vector data to new array"};
        }

        yoru_array_free(&vector->vector);
        vector->vector = new_array;
    }

    return yoru_array_set(&vector->vector, ++vector->highest_set_index, item);
}

YORU_API const size_t yoru_vector_size(const Yoru_Vector_t *vector)
{
    if (!vector)
    {
        return 0;
    }

    if (!vector->vector.slice.data)
    {
        return 0;
    }

    return vector->vector.size;
}

#endif // YORU_IMPLEMENTATION
#endif // __YORU_VECTOR_H__