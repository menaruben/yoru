#ifndef __YORU_LIST_H__
#define __YORU_LIST_H__

#include <stddef.h>
#include "../allocators/yoru_allocators.h"
#include "../asserts/yoru_asserts.h"
#include "../yoru_defs.h"

typedef struct ListNode_t
{
    void *data;
    struct ListNode_t *next;
    struct ListNode_t *prev;
} ListNode_t;

#define List_t(T)         \
    struct                \
    {                     \
        ListNode_t *head; \
        size_t size;      \
        T *t;             \
    }

YORU_HELPER void *listnode_new_impl(Yoru_Allocator_t *allocator);
YORU_HELPER ListNode_t *listnode_get_at(ListNode_t *head, size_t index, size_t list_size);

YORU_HELPER void list_destroy_impl(Yoru_Allocator_t *allocator, ListNode_t *head);

YORU_HELPER void list_insert_impl(Yoru_Allocator_t *allocator, ListNode_t *head, size_t index, void *data, size_t *list_size);
YORU_HELPER void list_append_impl(Yoru_Allocator_t *allocator, ListNode_t *head, void *data, size_t *list_size);
YORU_HELPER void list_prepend_impl(Yoru_Allocator_t *allocator, ListNode_t *head, void *data, size_t *list_size);

YORU_HELPER void *list_get_impl(ListNode_t *head, size_t index, size_t *list_size);
YORU_HELPER void list_remove_impl(Yoru_Allocator_t *allocator, ListNode_t *head, size_t index, size_t *list_size);

#define list_new(T, allocator_ptr) \
    {.head = (ListNode_t *)listnode_new_impl(allocator_ptr), .size = 0, .t = (T *)NULL}

#define list_destroy(list, allocator_ptr)              \
    do                                                 \
    {                                                  \
        list_destroy_impl(allocator_ptr, (list).head); \
        (list).head = NULL;                            \
        (list).size = 0;                               \
    } while (0)

#define list_insert(allocator_ptr, list_ptr, index, data_ptr) \
    list_insert_impl(allocator_ptr, (list_ptr).head, index, data_ptr, &(list_ptr).size)

#define list_append(allocator_ptr, list_ptr, data_ptr) \
    list_append_impl(allocator_ptr, (list_ptr).head, data_ptr, &(list_ptr).size)

#define list_prepend(allocator_ptr, list_ptr, data_ptr) \
    list_prepend_impl(allocator_ptr, (list_ptr).head, data_ptr, &(list_ptr).size)

#define list_get(list_ptr, index) \
    (typeof((list_ptr).t))list_get_impl((list_ptr).head, index, &(list_ptr).size)

#define list_remove(allocator_ptr, list_ptr, index) \
    list_remove_impl(allocator_ptr, (list_ptr).head, index, &(list_ptr).size)

YORU_HELPER void *listnode_new_impl(Yoru_Allocator_t *allocator)
{
    YORU_ASSERT_NOT_NULL(allocator);
    YORU_ASSERT_NOT_NULL(allocator->alloc);
    YORU_ASSERT_NOT_NULL(allocator->free);

    ListNode_t *head = (ListNode_t *)allocator->alloc(allocator->context, sizeof(ListNode_t));
    YORU_ASSERT_NOT_NULL(head);
    head->data = NULL;
    head->next = head; // circular
    head->prev = head; // circular
    return head;
}

YORU_HELPER ListNode_t *listnode_get_at(ListNode_t *head, size_t index, size_t list_size)
{
    YORU_ASSERT_NOT_NULL(head);
    YORU_ASSERT(index <= list_size, "Index out of bounds");
    ListNode_t *node = head->next;
    for (size_t i = 0; i < index; ++i)
    {
        node = node->next;
    }
    return node;
}

YORU_HELPER void list_destroy_impl(Yoru_Allocator_t *allocator, ListNode_t *head)
{
    YORU_ASSERT_NOT_NULL(allocator);
    YORU_ASSERT_NOT_NULL(allocator->free);
    YORU_ASSERT_NOT_NULL(head);

    ListNode_t *node = head->next;
    while (node != NULL && node != head)
    {
        ListNode_t *next = node->next;
        allocator->free(allocator->context, node);
        node = next;
    }
    allocator->free(allocator->context, head);
    head = NULL; // avoid dangling pointer
}

YORU_HELPER void list_insert_impl(
    Yoru_Allocator_t *allocator,
    ListNode_t *head,
    size_t index,
    void *data,
    size_t *list_size)
{
    YORU_ASSERT_NOT_NULL(allocator);
    YORU_ASSERT_NOT_NULL(allocator->alloc);
    YORU_ASSERT_NOT_NULL(allocator->free);
    YORU_ASSERT_NOT_NULL(head);
    YORU_ASSERT(index <= *list_size, "Index out of bounds");

    ListNode_t *node = listnode_get_at(head, index, *list_size);
    YORU_ASSERT_NOT_NULL(node);

    ListNode_t *new_node = (ListNode_t *)allocator->alloc(allocator->context, sizeof(ListNode_t));
    YORU_ASSERT_NOT_NULL(new_node);
    new_node->data = data;
    new_node->next = node;
    new_node->prev = node->prev;
    node->prev->next = new_node;
    node->prev = new_node;

    (*list_size)++;
}

YORU_HELPER void list_append_impl(Yoru_Allocator_t *allocator, ListNode_t *head, void *data, size_t *list_size)
{
    list_insert_impl(allocator, head, *list_size, data, list_size);
}

YORU_HELPER void list_prepend_impl(Yoru_Allocator_t *allocator, ListNode_t *head, void *data, size_t *list_size)
{
    list_insert_impl(allocator, head, 0, data, list_size);
}

YORU_HELPER void *list_get_impl(ListNode_t *head, size_t index, size_t *list_size)
{
    YORU_ASSERT_NOT_NULL(head);
    YORU_ASSERT(index < *list_size, "Index out of bounds");
    ListNode_t *node = listnode_get_at(head, index, *list_size);
    YORU_ASSERT_NOT_NULL(node);
    return node->data;
}

YORU_HELPER void list_remove_impl(Yoru_Allocator_t *allocator, ListNode_t *head, size_t index, size_t *list_size)
{
    YORU_ASSERT_NOT_NULL(allocator);
    YORU_ASSERT_NOT_NULL(allocator->free);
    YORU_ASSERT_NOT_NULL(head);
    YORU_ASSERT(index < *list_size, "Index out of bounds");
    ListNode_t *node = listnode_get_at(head, index, *list_size);
    YORU_ASSERT_NOT_NULL(node);
    YORU_ASSERT_NOT_NULL(node->prev);
    YORU_ASSERT_NOT_NULL(node->next);
    node->prev->next = node->next;
    node->next->prev = node->prev;
    allocator->free(allocator->context, node);
    node = NULL; // avoid dangling pointer
    (*list_size)--;
}

#endif