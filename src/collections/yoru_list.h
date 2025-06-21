#ifndef __YORU_LINKEDLIST_H__
#define __YORU_LINKEDLIST_H__

#include <stddef.h>
#include "../allocators/yoru_allocators.h"
#include "../math/yoru_math.h"
#include "../asserts/yoru_asserts.h"

#define List_T(type)                                                                    \
    typedef struct ListNode_##type                                                      \
    {                                                                                   \
        type value;                                                                     \
        struct ListNode_##type *next;                                                   \
    } ListNode_##type;                                                                  \
                                                                                        \
    typedef struct                                                                      \
    {                                                                                   \
        ListNode_##type *head;                                                          \
        size_t size;                                                                    \
    } List_##type;                                                                      \
                                                                                        \
    static inline List_##type List_##type##_Init()                                      \
    {                                                                                   \
        List_##type list = {.head = NULL, .size = 0};                                   \
        return list;                                                                    \
    }                                                                                   \
                                                                                        \
    static inline void List_##type##_PushFront(ArenaAllocator_t *arena,                 \
                                               List_##type *list, type value)           \
    {                                                                                   \
        ListNode_##type *node =                                                         \
            (ListNode_##type *)ArenaAllocator_Allocate(arena, sizeof(ListNode_##type)); \
        ASSERT_NOT_EQUAL(node, NULL);                                                   \
        node->value = value;                                                            \
        node->next = list->head;                                                        \
        list->head = node;                                                              \
        list->size++;                                                                   \
    }                                                                                   \
                                                                                        \
    static inline void List_##type##_PushBack(ArenaAllocator_t *arena,                  \
                                              List_##type *list,                        \
                                              type value)                               \
    {                                                                                   \
        ListNode_##type *node =                                                         \
            (ListNode_##type *)ArenaAllocator_Allocate(arena, sizeof(ListNode_##type)); \
        ASSERT_NOT_EQUAL(node, NULL);                                                   \
        node->value = value;                                                            \
        node->next = NULL;                                                              \
        if (list->head == NULL)                                                         \
        {                                                                               \
            list->head = node;                                                          \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            ListNode_##type *cur = list->head;                                          \
            while (cur->next != NULL)                                                   \
                cur = cur->next;                                                        \
            cur->next = node;                                                           \
        }                                                                               \
        list->size++;                                                                   \
    }                                                                                   \
                                                                                        \
    static inline void List_##type##_Insert(ArenaAllocator_t *arena,                    \
                                            List_##type *list,                          \
                                            size_t index,                               \
                                            type value)                                 \
    {                                                                                   \
        /* allow inserting at end because its a list */                                 \
        index = usize_modulo(index, list->size + 1);                                    \
        if (index == 0)                                                                 \
        {                                                                               \
            List_##type##_PushFront(arena, list, value);                                \
            return;                                                                     \
        }                                                                               \
        ListNode_##type *prev = list->head;                                             \
        for (size_t i = 0; i < index - 1; ++i)                                          \
        {                                                                               \
            prev = prev->next;                                                          \
        }                                                                               \
        ListNode_##type *node =                                                         \
            (ListNode_##type *)ArenaAllocator_Allocate(arena, sizeof(ListNode_##type)); \
        ASSERT_NOT_EQUAL(node, NULL);                                                   \
        node->value = value;                                                            \
        node->next = prev->next;                                                        \
        prev->next = node;                                                              \
        list->size++;                                                                   \
    }                                                                                   \
                                                                                        \
    static inline void List_##type##_Remove(List_##type *list, size_t index)            \
    {                                                                                   \
        if (list->size == 0)                                                            \
            return;                                                                     \
        index = usize_modulo(index, list->size);                                        \
        ListNode_##type *to_remove;                                                     \
        if (index == 0)                                                                 \
        {                                                                               \
            to_remove = list->head;                                                     \
            list->head = list->head->next;                                              \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            ListNode_##type *prev = list->head;                                         \
            for (size_t i = 0; i < index - 1; ++i)                                      \
            {                                                                           \
                prev = prev->next;                                                      \
            }                                                                           \
            to_remove = prev->next;                                                     \
            prev->next = to_remove->next;                                               \
        }                                                                               \
        list->size--;                                                                   \
    }                                                                                   \
                                                                                        \
    static inline type *List_##type##_Get(List_##type *list, size_t index)              \
    {                                                                                   \
        if (list->size == 0)                                                            \
            return NULL;                                                                \
        index = usize_modulo(index, list->size);                                        \
        ListNode_##type *cur = list->head;                                              \
        for (size_t i = 0; i < index; ++i)                                              \
        {                                                                               \
            cur = cur->next;                                                            \
        }                                                                               \
        return cur ? &cur->value : NULL;                                                \
    }

#endif
