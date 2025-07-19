#ifndef __YORU_LIST_H__
#define __YORU_LIST_H__

#include <stddef.h>
#include "../memory/yoru_memory.h"
#include "../asserts/yoru_asserts.h"
#include "../yoru_defs.h"
#include "../results/yoru_results.h"
#include "../utils/yoru_utils.h"

typedef struct Yoru_ListNode_t
{
    void *data;
    struct Yoru_ListNode_t *next;
    struct Yoru_ListNode_t *prev;
} Yoru_ListNode_t;

typedef struct Yoru_List_t
{
    Yoru_ListNode_t *head;
    size_t size;
} Yoru_List_t;

/// @brief Initialize a linked list.
/// @param list Pointer to the Yoru_List_t structure to initialize.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the list is NULL.
YORU_API Yoru_Error_t yoru_list_init(Yoru_List_t *list);

/// @brief Free the linked list and its nodes.
/// @param list Pointer to the Yoru_List_t structure to free.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the list is NULL.
YORU_API Yoru_Error_t yoru_list_free(Yoru_List_t *list);

/// @brief Get an item from the linked list at a specific index.
/// @param list Pointer to the Yoru_List_t structure.
/// @param index The index of the item to get.
/// @param item Pointer to the location where the item will be stored.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the list is NULL, item is NULL, or index is out of bounds.
YORU_API Yoru_Error_t yoru_list_get(const Yoru_List_t *list, const size_t index, YORU_OUT void *item);

/// @brief Set an item in the linked list at a specific index.
/// @param list Pointer to the Yoru_List_t structure.
/// @param index The index at which to set the item.
/// @param item Pointer to the item to set in the list.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the list is NULL, item is NULL, or index is out of bounds.
YORU_API Yoru_Error_t yoru_list_set(Yoru_List_t *list, const size_t index, const void *item);

/// @brief Remove an item from the linked list at a specific index.
/// @param list Pointer to the Yoru_List_t structure.
/// @param index The index of the item to remove.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the list is NULL, or index is out of bounds.
YORU_API Yoru_Error_t yoru_list_remove(Yoru_List_t *list, const size_t index);

/// @brief Prepend an item to the linked list.
/// @param list Pointer to the Yoru_List_t structure.
/// @param item Pointer to the item to prepend to the list.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the list is NULL, or item is NULL.
YORU_API Yoru_Error_t yoru_list_prepend(Yoru_List_t *list, const void *item);

/// @brief Append an item to the linked list.
/// @param list Pointer to the Yoru_List_t structure.
/// @param item Pointer to the item to append to the list.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the list is NULL, or item is NULL.
YORU_API Yoru_Error_t yoru_list_append(Yoru_List_t *list, const void *item);

#ifdef YORU_IMPLEMENTATION
YORU_API Yoru_Error_t yoru_list_init(Yoru_List_t *list)
{
    if (!list)
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(list) " cannot be NULL"};

    list->head = NULL;
    list->size = 0;
    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
}

YORU_API Yoru_Error_t yoru_list_free(Yoru_List_t *list)
{
    if (!list)
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(list) " cannot be NULL"};
    Yoru_ListNode_t *node = list->head;
    while (node)
    {
        Yoru_ListNode_t *next = node->next;
        if (node->data)
        {
            Yoru_Slice_t slice = {.data = node->data, .offset = 0, .capacity = 0};
            yoru_heap_free(&slice);
        }
        Yoru_Slice_t node_slice = {.data = node, .offset = 0, .capacity = 0};
        yoru_heap_free(&node_slice);
        node = next;
    }
    list->head = NULL;
    list->size = 0;
    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
}

YORU_API Yoru_Error_t yoru_list_get(const Yoru_List_t *list, const size_t index, YORU_OUT void *item)
{
    if (!list)
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(list) " cannot be NULL"};
    if (!item)
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(item) " cannot be NULL"};
    if (index >= list->size)
        return (Yoru_Error_t){.type = YORU_ERR_OUT_OF_BOUNDS, .message = YORU_NAMEOF(index) " is out of bounds."};

    Yoru_ListNode_t *node = list->head;
    for (size_t i = 0; i < index; ++i)
        node = node->next;
    if (!node || !node->data)
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = "Node or node data is NULL"};
    memcpy(item, node->data, sizeof(*item));
    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
}

YORU_API Yoru_Error_t yoru_list_set(Yoru_List_t *list, const size_t index, const void *item)
{
    if (!list)
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(list) " cannot be NULL"};

    if (!item)
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(item) " cannot be NULL"};

    if (index >= list->size)
        return (Yoru_Error_t){.type = YORU_ERR_OUT_OF_BOUNDS, .message = YORU_NAMEOF(index) " is out of bounds."};

    Yoru_ListNode_t *node = list->head;
    for (size_t i = 0; i < index; ++i)
        node = node->next;

    if (!node || !node->data)
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = "Node or node data is NULL"};

    memcpy(node->data, item, sizeof(*item));
    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
}

YORU_API Yoru_Error_t yoru_list_prepend(Yoru_List_t *list, const void *item)
{
    if (!list)
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(list) " cannot be NULL"};
    if (!item)
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(item) " cannot be NULL"};

    Yoru_ListNode_t *node = NULL;
    Yoru_Slice_t node_slice = {0};
    Yoru_Error_t err = yoru_heap_alloc(sizeof(Yoru_ListNode_t), &node_slice);
    if (err.type != YORU_OK)
        return err;
    node = (Yoru_ListNode_t *)node_slice.data;

    Yoru_Slice_t data_slice = {0};
    err = yoru_heap_alloc(sizeof(*item), &data_slice);
    if (err.type != YORU_OK)
    {
        yoru_heap_free(&node_slice);
        return err;
    }
    node->data = data_slice.data;
    memcpy(node->data, item, sizeof(*item));
    node->prev = NULL;
    node->next = list->head;
    if (list->head)
        list->head->prev = node;
    list->head = node;
    list->size++;
    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
}

YORU_API Yoru_Error_t yoru_list_append(Yoru_List_t *list, const void *item)
{
    if (!list)
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(list) " cannot be NULL"};

    if (!item)
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(item) " cannot be NULL"};

    Yoru_ListNode_t *node = NULL;
    Yoru_Slice_t node_slice = {0};
    Yoru_Error_t err = yoru_heap_alloc(sizeof(Yoru_ListNode_t), &node_slice);
    if (err.type != YORU_OK)
        return err;
    node = (Yoru_ListNode_t *)node_slice.data;

    Yoru_Slice_t data_slice = {0};
    err = yoru_heap_alloc(sizeof(*item), &data_slice);
    if (err.type != YORU_OK)
    {
        yoru_heap_free(&node_slice);
        return err;
    }
    node->data = data_slice.data;
    memcpy(node->data, item, sizeof(*item));
    node->next = NULL;

    if (!list->head)
    {
        node->prev = NULL;
        list->head = node;
    }
    else
    {
        Yoru_ListNode_t *tail = list->head;
        while (tail->next)
            tail = tail->next;
        tail->next = node;
        node->prev = tail;
    }
    list->size++;
    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
}

YORU_API Yoru_Error_t yoru_list_remove(Yoru_List_t *list, const size_t index)
{
    if (!list)
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(list) " cannot be NULL"};

    if (index >= list->size)
        return (Yoru_Error_t){.type = YORU_ERR_OUT_OF_BOUNDS, .message = YORU_NAMEOF(index) " is out of bounds."};

    Yoru_ListNode_t *node = list->head;
    for (size_t i = 0; i < index; ++i)
        node = node->next;

    if (!node)
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = "Node is NULL"};

    if (node->prev)
        node->prev->next = node->next;
    else
        list->head = node->next;

    if (node->next)
        node->next->prev = node->prev;

    if (node->data)
    {
        Yoru_Slice_t data_slice = {.data = node->data, .offset = 0, .capacity = 0};
        yoru_heap_free(&data_slice);
    }

    Yoru_Slice_t node_slice = {.data = node, .offset = 0, .capacity = 0};
    yoru_heap_free(&node_slice);
    list->size--;
    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
}

#endif // YORU_IMPLEMENTATION
#endif