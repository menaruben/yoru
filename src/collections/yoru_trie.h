#ifndef __YORU_TRIE_H__
#define __YORU_TRIE_H__

/*
    A Trie is a tree-like data structure to store
    key value pairs, where the keys are usually strings.

    For more information, see:
    https://en.wikipedia.org/wiki/Trie

    My implementation of a Trie is NOT tied to a specific type,
    so you can store any type of value in it.
*/

#include <stddef.h>
#include <string.h>
#include "../inttypes/yoru_inttypes.h"
#include "../allocators/yoru_allocators.h"
#include "../yoru_defs.h"
#include "../results/yoru_results.h"
#include "../utils/yoru_utils.h"

typedef struct TrieNode_t
{
    u8 key;
    struct TrieNode_t *children[256];
    void *value;
} TrieNode_t;

YORU_HELPER void *trie_node_create_impl(Yoru_Allocator_t *allocator, size_t node_size, u8 key);

YORU_HELPER void trie_put_impl(void *root, const u8 *key, void *value, size_t node_size, Yoru_Allocator_t *allocator);
YORU_HELPER Yoru_Error_t trie_put_try_impl(void *root, const u8 *key, void *value, size_t node_size, Yoru_Allocator_t *allocator);

YORU_HELPER void *trie_get_impl(void *root, const u8 *key);
YORU_HELPER Yoru_Result_t trie_get_try_impl(void *root, const u8 *key);

YORU_HELPER void trie_remove_impl(void *root, const u8 *key);
YORU_HELPER Yoru_Error_t trie_remove_try_impl(void *root, const u8 *key);

YORU_HELPER void trie_destroy_impl(Yoru_Allocator_t *allocator, void *node, size_t node_size);

#define trie_new(allocator_ptr) \
    (TrieNode_t *)trie_node_create_impl(allocator_ptr, sizeof(TrieNode_t), 0);

#define trie_get(T_ptr, node_ptr, key) \
    (T_ptr) trie_get_impl((void *)(node_ptr), (const u8 *)(key))

#define trie_get_try(node_ptr, key) \
    (Yoru_Result_t) trie_get_try_impl((void *)(node_ptr), (const u8 *)(key))

#define trie_get_as(T, node_ptr, key) \
    (T *)trie_get_impl((void *)(node_ptr), (const u8 *)(key))

#define trie_get_as_try(T, node_ptr, key) \
    (Yoru_Result_t) trie_get_try_impl((void *)(node_ptr), (const u8 *)(key))

#define trie_put(node_ptr, key, value_ptr, allocator_ptr) \
    (void)trie_put_impl((void *)(node_ptr), (const u8 *)(key), (void *)(value_ptr), sizeof(*(node_ptr)), allocator_ptr)

#define trie_put_try(node_ptr, key, value_ptr, allocator_ptr) \
    (Yoru_Error_t) trie_put_try_impl((void *)(node_ptr), (const u8 *)(key), (void *)(value_ptr), sizeof(*(node_ptr)), allocator_ptr)

#define trie_remove(node_ptr, key) \
    (void)trie_remove_impl((void *)(node_ptr), (const u8 *)(key), sizeof(*(node_ptr)))

#define trie_remove_try_(node_ptr, key) \
    (Yoru_Error_t) trie_remove_try_impl((void *)(node_ptr), (const u8 *)(key), sizeof(*(node_ptr)))

#define trie_remove_try(node_ptr, key) \
    (Yoru_Error_t) trie_remove_try_impl((void *)(node_ptr), (const u8 *)(key))

#define trie_destroy(node_ptr, allocator_ptr) \
    (void)trie_destroy_impl(allocator_ptr, (void *)(node_ptr), sizeof(*(node_ptr)))

#ifdef YORU_IMPLEMENTATION
YORU_HELPER void *trie_node_create_impl(Yoru_Allocator_t *allocator, size_t node_size, u8 key)
{
    YORU_ASSERT_NOT_NULL(allocator);
    TrieNode_t *node = (TrieNode_t *)allocator->alloc(allocator->context, node_size);
    node->key = key;
    for (int i = 0; i < 256; ++i)
        node->children[i] = NULL;

    node->value = NULL;
    return node;
}

YORU_HELPER void trie_put_impl(
    void *root,
    const u8 *key,
    void *value,
    size_t node_size,
    Yoru_Allocator_t *allocator)
{
    YORU_ASSERT_NOT_NULL(allocator);
    YORU_ASSERT_NOT_NULL(root);
    YORU_ASSERT_NOT_NULL(key);

    TrieNode_t *node = (TrieNode_t *)root;
    for (size_t i = 0; key[i]; ++i)
    {
        u8 c = key[i];
        if (!node->children[c])
            node->children[c] = (TrieNode_t *)trie_node_create_impl(allocator, node_size, c);
        node = node->children[c];
    }
    node->value = value;
}

YORU_HELPER Yoru_Error_t trie_put_try_impl(
    void *root,
    const u8 *key,
    void *value,
    size_t node_size,
    Yoru_Allocator_t *allocator)
{
    if (!allocator || !root || !key)
        return YORU_ERR_ARGUMENT_NULL;

    trie_put_impl(root, key, value, node_size, allocator);
    return YORU_OK;
}

YORU_HELPER void *trie_get_impl(void *root, const u8 *key)
{
    TrieNode_t *node = (TrieNode_t *)root;
    for (size_t i = 0; key[i]; ++i)
    {
        u8 c = key[i];
        if (!node->children[c])
            return NULL;
        node = node->children[c];
    }
    return node->value;
}

YORU_HELPER Yoru_Result_t trie_get_try_impl(void *root, const u8 *key)
{
    if (!root)
        return (Yoru_Result_t){.value = NULL, .err = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(root) " is NULL"};

    if (!key)
        return (Yoru_Result_t){.value = NULL, .err = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(key) " is NULL"};

    void *value = trie_get_impl(root, key);
    if (!value)
        return (Yoru_Result_t){.value = NULL, .err = YORU_ERR_KEY_NOT_FOUND, .message = "Key not found"};

    return (Yoru_Result_t){.value = value, .err = YORU_OK};
}

YORU_HELPER void trie_remove_impl(void *root, const u8 *key)
{
    YORU_ASSERT_NOT_NULL(root);
    YORU_ASSERT_NOT_NULL(key);
    TrieNode_t *node = (TrieNode_t *)root;
    for (size_t i = 0; key[i]; ++i)
    {
        u8 c = key[i];
        if (!node->children[c])
            return;

        node = node->children[c];
    }
    node->value = NULL;
}

YORU_HELPER Yoru_Error_t trie_remove_try_impl(void *root, const u8 *key)
{
    if (!root || !key)
        return YORU_ERR_ARGUMENT_NULL;

    trie_remove_impl(root, key);
    return YORU_OK;
}

YORU_HELPER void trie_destroy_impl(Yoru_Allocator_t *allocator, void *node, size_t node_size)
{
    YORU_ASSERT_NOT_NULL(allocator);
    YORU_ASSERT_NOT_NULL(node);
    TrieNode_t *n = (TrieNode_t *)node;

    // Free all children nodes recursively
    for (int i = 0; i < 256; ++i)
    {
        if (n->children[i])
            trie_destroy_impl(allocator, n->children[i], node_size);
    }
    allocator->free(allocator->context, node);
}
#endif // YORU_IMPLEMENTATION

#endif