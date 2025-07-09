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

typedef struct TrieNode_t
{
    u8 key;
    struct TrieNode_t *children[256];
    void *value;
} TrieNode_t;

YORU_HELPER void *trie_node_create_impl(Yoru_Allocator_t *allocator, size_t node_size, u8 key);
YORU_HELPER void trie_put_impl(void *root, const u8 *key, void *value, size_t node_size, Yoru_Allocator_t *allocator);
YORU_HELPER void *trie_get_impl(void *root, const u8 *key);
YORU_HELPER void trie_remove_impl(void *root, const u8 *key);
YORU_HELPER void trie_destroy_impl(Yoru_Allocator_t *allocator, void *node, size_t node_size);

#define trie_new(allocator_ptr) \
    (TrieNode_t *)trie_node_create_impl(allocator_ptr, sizeof(TrieNode_t), 0);

#define trie_get(node_ptr, key) \
    (typeof((node_ptr)->value))trie_get_impl((void *)(node_ptr), (const u8 *)(key))

#define trie_get_as(T, node_ptr, key) \
    (T *)trie_get_impl((void *)(node_ptr), (const u8 *)(key))

#define trie_put(node_ptr, key, value_ptr, allocator_ptr) \
    (void)trie_put_impl((void *)(node_ptr), (const u8 *)(key), (void *)(value_ptr), sizeof(*(node_ptr)), allocator_ptr)

#define trie_remove(node_ptr, key) \
    (void)trie_remove_impl((void *)(node_ptr), (const u8 *)(key), sizeof(*(node_ptr)))

#define trie_destroy(node_ptr, allocator_ptr) \
    (void)trie_destroy_impl(allocator_ptr, (void *)(node_ptr), sizeof(*(node_ptr)))

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

#endif