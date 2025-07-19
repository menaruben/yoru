#ifndef __YORU_ARRAY_H__
#define __YORU_ARRAY_H__

#include <stddef.h>
#include <string.h>
#include "../memory/yoru_memory.h"
#include "../yoru_defs.h"
#include "../utils/yoru_utils.h"
#include "../results/yoru_results.h"
#include "../math/yoru_math.h"

typedef enum Yoru_ArrayIndexStrategy_t
{
    YORU_ARRAY_INDEX_STRICT,      // strict index checking, out of bounds access will return an error (Default)
    YORU_ARRAY_INDEX_WRAP_AROUND, // wrap around index, so if index is out of bounds it will wrap around to the start
} Yoru_ArrayIndexStrategy_t;

/// @brief Represents a dynamic array (array) with a slice of memory, item size, and length.
typedef struct Yoru_Array_t
{
    Yoru_Slice_t slice;                       // memory slice that holds the array data
    size_t item_size;                         // size of each item in bytes
    size_t size;                              // total number of items that can be stored, can also be calculated as slice.capacity / item_size
    Yoru_ArrayIndexStrategy_t index_strategy; // strategy for handling out of bounds access
} Yoru_Array_t;

/// @brief Initialize a dynamic array with a specified item size and initial capacity.
/// @param item_size Size of each item in bytes.
/// @param item_count Initial number of items to allocate space for.
/// @param array Pointer to the Yoru_Array_t structure to initialize.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the array is NULL or item_size is invalid.
YORU_API Yoru_Error_t yoru_array_init(const size_t item_size, const size_t item_count, YORU_OUT Yoru_Array_t *array);

/// @brief Free the memory allocated for the dynamic array.
/// @param array Pointer to the Yoru_Array_t structure to free.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the array is NULL.
YORU_API Yoru_Error_t yoru_array_free(Yoru_Array_t *array);

/// @brief Set an item in the dynamic array at a specified index.
/// @param array Pointer to the Yoru_Array_t structure.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the array is NULL, item is NULL, or index is out of bounds.
YORU_API const size_t yoru_array_size(const Yoru_Array_t *array);

/// @brief Set an item in the dynamic array at a specified index.
/// @param array Pointer to the Yoru_Array_t structure.
/// @param index Index at which to set the item.
/// @param item Pointer to the item to set in the array.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the array is NULL, item is NULL, or index is out of bounds.
YORU_API Yoru_Error_t yoru_array_set(Yoru_Array_t *array, size_t index, const void *item);

/// @brief Get an item from the dynamic array at a specified index.
/// @param array Pointer to the Yoru_Array_t structure.
/// @param index Index at which to get the item.
/// @param out_item Pointer to the location where the item will be stored.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the array is NULL, out_item is NULL, or index is out of bounds.
YORU_API Yoru_Error_t yoru_array_get(const Yoru_Array_t *array, size_t index, YORU_OUT void *out_item);

/// @brief Copy items from one array to another.
/// @param src Pointer to the source Yoru_Array_t structure.
/// @param dest Pointer to the destination Yoru_Array_t structure.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the source or destination arrays are NULL, or if their item sizes do not match.
YORU_API Yoru_Error_t yoru_array_copy(const Yoru_Array_t *src, Yoru_Array_t *dest);

/// @brief Move items in the array to the left by a specified number of items.
/// @param array Pointer to the Yoru_Array_t structure.
/// @param item_count Number of items to move to the left.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the array is NULL or item_count exceeds the array size.
YORU_API Yoru_Error_t yoru_array_move_left(Yoru_Array_t *array, size_t item_count);

/// @brief Move items in the array to the right by a specified number of items.
/// @param array Pointer to the Yoru_Array_t structure.
/// @param item_count Number of items to move to the right.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the array is NULL or item_count exceeds the array size.
YORU_API Yoru_Error_t yoru_array_move_right(Yoru_Array_t *array, size_t item_count);

#ifdef YORU_IMPLEMENTATION

YORU_API Yoru_Error_t yoru_array_init(const size_t item_size, const size_t item_count, YORU_OUT Yoru_Array_t *array)
{
    if (!array)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(array) " cannot be NULL"};
    }

    if (item_size <= 0)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_INVALID, .message = YORU_NAMEOF(item_size) " must be greater than 0"};
    }

    Yoru_Error_t slice_res = yoru_heap_alloc(item_count * item_size, &array->slice);
    if (slice_res.type != YORU_OK)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ALLOC, .message = "Couldn't allocate memory for the array slice"};
    }

    array->item_size = item_size;
    array->size = item_count;
    array->index_strategy = YORU_ARRAY_INDEX_STRICT;
    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
}

YORU_API Yoru_Error_t yoru_array_free(Yoru_Array_t *array)
{
    (void)yoru_heap_free(&array->slice);
    array->item_size = 0;
    array->size = 0;
    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
}

YORU_API Yoru_Error_t yoru_array_set(Yoru_Array_t *array, size_t index, const void *item)
{
    if (!array)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(array) " cannot be NULL"};
    }

    if (!item)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(item) " cannot be NULL"};
    }

    if (array->index_strategy == YORU_ARRAY_INDEX_STRICT && index >= array->size)
    {
        return (Yoru_Error_t){.type = YORU_ERR_OUT_OF_BOUNDS, .message = YORU_NAMEOF(index) " is out of bounds."};
    }

    if (array->index_strategy == YORU_ARRAY_INDEX_WRAP_AROUND)
    {
        index = usize_modulo(index, array->size);
    }

    char *target = (char *)array->slice.data + (index * array->item_size);
    if (!item)
    {
        (void)memset(target, 0, array->item_size);
    }
    else
    {
        (void)memcpy(target, item, array->item_size);
    }

    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
}

YORU_API Yoru_Error_t yoru_array_get(const Yoru_Array_t *array, size_t index, YORU_OUT void *out_item)
{
    if (!array)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(array) " cannot be NULL"};
    }

    if (!out_item)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(out_item) " cannot be NULL"};
    }

    if (array->index_strategy == YORU_ARRAY_INDEX_STRICT && index >= array->size)
    {
        return (Yoru_Error_t){.type = YORU_ERR_OUT_OF_BOUNDS, .message = YORU_NAMEOF(index) " is out of bounds."};
    }

    if (array->index_strategy == YORU_ARRAY_INDEX_WRAP_AROUND)
    {
        index = usize_modulo(index, array->size);
    }

    char *source = (char *)array->slice.data + (index * array->item_size);
    if (!source)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = "source item is NULL"};
    }

    (void)memcpy(out_item, source, array->item_size);
    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
}

YORU_API Yoru_Error_t yoru_array_copy(const Yoru_Array_t *src, Yoru_Array_t *dest)
{
    if (!src)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(src) " cannot be NULL"};
    }

    if (!dest)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(dest) " cannot be NULL"};
    }

    if (src->item_size != dest->item_size)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_INVALID, .message = "Source and destination item sizes must match"};
    }

    if (src->size > dest->size)
    {
        return (Yoru_Error_t){.type = YORU_ERR_OUT_OF_BOUNDS, .message = "Source array size is larger than destination array size"};
    }

    char *src_data = (char *)src->slice.data;
    char *dest_data = (char *)dest->slice.data;
    size_t copy_size = src->size * src->item_size;
    if (!src_data)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = "Source data is NULL"};
    }

    if (!dest_data)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = "Destination data is NULL"};
    }

    (void)memcpy(dest_data, src_data, copy_size);
    char *non_set_data = (char *)dest->slice.data + copy_size;
    size_t remaining_size = dest->slice.capacity - copy_size;
    if (remaining_size > 0)
    {
        (void)memset(non_set_data, 0, remaining_size);
    }

    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
}

YORU_API Yoru_Error_t yoru_array_move_left(Yoru_Array_t *array, size_t item_count)
{
    if (!array)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(array) " cannot be NULL"};
    }

    if (item_count >= array->size)
    {
        return (Yoru_Error_t){.type = YORU_ERR_OUT_OF_BOUNDS, .message = "Item count exceeds array size"};
    }

    char *data = (char *)array->slice.data;
    size_t move_size = item_count * array->item_size;
    (void)memmove(data, data + move_size, array->slice.capacity - move_size); // move data to left (pop item_count items from front)
    (void)memset(data + array->slice.capacity - move_size, 0, move_size);     // zero init the now unused space at the end
    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
}

YORU_API Yoru_Error_t yoru_array_move_right(Yoru_Array_t *array, size_t item_count)
{
    if (!array)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(array) " cannot be NULL"};
    }

    if (item_count >= array->size)
    {
        return (Yoru_Error_t){.type = YORU_ERR_OUT_OF_BOUNDS, .message = "Item count exceeds array size"};
    }

    char *data = (char *)array->slice.data;
    size_t move_size = item_count * array->item_size;
    (void)memmove(data + move_size, data, array->slice.capacity - move_size); // move data to right (push item_count items to end)
    (void)memset(data, 0, move_size);                                         // zero init the now unused space at the start
    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
}

#endif // YORU_IMPLEMENTATION
#endif // __YORU_ARRAY_H__