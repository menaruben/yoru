#ifndef __YORU_TRIE_H__
#define __YORU_TRIE_H__

/*
    A Trie is a tree-like data structure to store
    key value pairs, where the keys are usually strings.

    For more information, see:
    https://en.wikipedia.org/wiki/Trie

    My implementation of a Trie is NOT tied to a specific type,
    so you can store any type of value in it.

    This decision was made because I think that a Trie is a good choice
    for storing key-value pairs of different types, like a JSON or XML object
    since you do not have to use a hash function to store values to a key.

    Typically hash functions do/should not collide with each other within the same type, but
    when you have different types, this gets complicated. Therefore I decided to commit to a
    flexible Trie implementation.
*/

#include <stddef.h>
#include <string.h>
#include "../inttypes/yoru_inttypes.h"
#include "../allocators/yoru_allocators.h"

typedef struct TrieNode_t
{
    u8 key;
    struct TrieNode_t *children[256];
    void *value;
} TrieNode_t;

static inline void *trie_node_create_impl(size_t node_size, u8 key, Allocator_t *allocator);
static inline void trie_put_impl(void *root, const u8 *key, void *value, size_t node_size, Allocator_t *allocator);
static inline void *trie_get_impl(void *root, const u8 *key);
static inline void trie_remove_impl(void *root, const u8 *key);
static inline void trie_destroy_impl(void *node, size_t node_size, Allocator_t *allocator);

#define trie_new(allocator) \
    (TrieNode_t *)trie_node_create_impl(sizeof(TrieNode_t), 0, allocator);

#define trie_get(node, key) \
    (typeof((node)->value))trie_get_impl((void *)(node), (const u8 *)(key))

#define trie_get_as(T, node, key) \
    (T *)trie_get_impl((void *)(node), (const u8 *)(key))

#define trie_put(node, key, value, allocator) \
    (void)trie_put_impl((void *)(node), (const u8 *)(key), (void *)(value), sizeof(*(node)), allocator)

#define trie_remove(node, key) \
    (void)trie_remove_impl((void *)(node), (const u8 *)(key), sizeof(*(node)))

#define trie_destroy(node, allocator) \
    (void)trie_destroy_impl((void *)(node), sizeof(*(node)), allocator)

static inline void *trie_node_create_impl(size_t node_size, u8 key, Allocator_t *allocator)
{
    ASSERT_NOT_NULL(allocator);
    TrieNode_t *node = (TrieNode_t *)allocator->alloc(allocator->context, node_size);
    node->key = key;
    for (int i = 0; i < 256; ++i)
        node->children[i] = NULL;

    node->value = NULL;
    return node;
}

static inline void trie_put_impl(
    void *root,
    const u8 *key,
    void *value,
    size_t node_size,
    Allocator_t *allocator)
{
    ASSERT_NOT_NULL(allocator);
    ASSERT_NOT_NULL(root);
    ASSERT_NOT_NULL(key);

    TrieNode_t *node = (TrieNode_t *)root;
    for (size_t i = 0; key[i]; ++i)
    {
        u8 c = key[i];
        if (!node->children[c])
            node->children[c] = (TrieNode_t *)trie_node_create_impl(node_size, c, allocator);
        node = node->children[c];
    }
    node->value = value;
}

static inline void *trie_get_impl(void *root, const u8 *key)
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

static inline void trie_remove_impl(void *root, const u8 *key)
{
    ASSERT_NOT_NULL(root);
    ASSERT_NOT_NULL(key);
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

static inline void trie_destroy_impl(void *node, size_t node_size, Allocator_t *allocator)
{
    ASSERT_NOT_NULL(allocator);
    ASSERT_NOT_NULL(node);
    TrieNode_t *n = (TrieNode_t *)node;

    // Free all children nodes recursively
    for (int i = 0; i < 256; ++i)
    {
        if (n->children[i])
            trie_destroy_impl(n->children[i], node_size, allocator);
    }
    allocator->free(allocator->context, node);
}

#endif