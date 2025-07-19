#ifndef __YORU_TRIE_H__
#define __YORU_TRIE_H__

/*
    A Trie is a tree-like data structure to store
    key value pairs, where the keys are usually strings.

    For more information, see:
    https://en.wikipedia.org/wiki/Trie
*/

#include <stddef.h>
#include <string.h>
#include "../inttypes/yoru_inttypes.h"
#include "../memory/yoru_memory.h"
#include "../yoru_defs.h"
#include "../results/yoru_results.h"
#include "../utils/yoru_utils.h"

typedef struct Yoru_TrieNode_t
{
    u8 key;
    struct Yoru_TrieNode_t *children[256];
    void *value;
    size_t item_size;
} Yoru_TrieNode_t;

/// @brief Initialize a trie node.
/// @param root The root node of the trie to initialize.
/// @param item_size The size of the items that will be stored in the trie.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the root is NULL.
YORU_API Yoru_Error_t yoru_trie_init(Yoru_TrieNode_t *root, const size_t item_size);

/// @brief Free the trie's children recursively and the value if it exists.
/// @param root The root node of the trie to free.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the root is NULL.
/// @note This function does not free the root node itself, only its children and value.
///      The caller is responsible for freeing the root node if needed since it is not sure where the root node is allocated.
///      It could be on the stack or heap.
YORU_API Yoru_Error_t yoru_trie_free(Yoru_TrieNode_t *root);

/// @brief Get an item from the trie by its key.
/// @param root The root node of the trie to search in.
/// @param key The key to search for in the trie.
/// @param item Pointer to the item to store the result in.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the key is not found, or if the root is NULL.
YORU_API Yoru_Error_t yoru_trie_get(const Yoru_TrieNode_t *root, const char *key, YORU_OUT void *item);

/// @brief Set an item in the trie by its key.
/// @param root The root node of the trie to set the item in.
/// @param key The key to set the item for in the trie.
/// @param item Pointer to the item to store in the trie.
/// @return Yoru_Error_t Returns YORU_OK on success, or an error if the key is invalid, or if the root is NULL.
YORU_API Yoru_Error_t yoru_trie_set(const Yoru_TrieNode_t *root, const char *key, YORU_OUT void *item);

#ifdef YORU_IMPLEMENTATION

YORU_API Yoru_Error_t yoru_trie_init(Yoru_TrieNode_t *root, const size_t item_size)
{
    if (!root)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(root) " cannot be NULL"};
    }

    root->key = 0;
    root->value = NULL;
    for (size_t i = 0; i < 256; ++i)
    {
        root->children[i] = NULL;
    }

    root->item_size = item_size;
    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
}

YORU_API Yoru_Error_t yoru_trie_free(Yoru_TrieNode_t *root)
{
    if (!root)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(root) " cannot be NULL"};
    }

    for (size_t i = 0; i < 256; ++i)
    {
        if (root->children[i])
        {
            // Recursively free child nodes, may want to turn into iterative for large tries to avoid stack overflow
            Yoru_Error_t err = yoru_trie_free(root->children[i]);
            if (err.type != YORU_OK)
                return err;
        }
    }

    if (root->value)
    {
        Yoru_Slice_t slice = {.data = root->value, .offset = 0, .capacity = 0};
        yoru_heap_free(&slice);
    }

    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
}

YORU_API Yoru_Error_t yoru_trie_get(const Yoru_TrieNode_t *root, const char *key, YORU_OUT void *item)
{
    if (!root)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(root) " cannot be NULL"};
    }

    if (!key || !*key)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_INVALID, .message = "Key cannot be NULL or empty"};
    }

    if (!item)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(item) " cannot be NULL"};
    }

    const Yoru_TrieNode_t *current = root;
    while (*key)
    {
        struct Yoru_TrieNode_t *next = current->children[(unsigned char)*key];
        if (!next)
        {
            return (Yoru_Error_t){.type = YORU_ERR_KEY_NOT_FOUND, .message = "Key not found in trie"};
        }

        current = next;
        key++;
    }

    if (current->value)
    {
        memcpy(item, current->value, root->item_size);
        return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
    }
    else
    {
        return (Yoru_Error_t){.type = YORU_ERR_KEY_NOT_FOUND, .message = "Key found but no value associated"};
    }
}

YORU_API Yoru_Error_t yoru_trie_set(const Yoru_TrieNode_t *root, const char *key, YORU_OUT void *item)
{
    if (!root)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(root) " cannot be NULL"};
    }

    if (!key || !*key)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_INVALID, .message = "Key cannot be NULL or empty"};
    }

    if (!item)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(item) " cannot be NULL"};
    }

    Yoru_TrieNode_t *current = (Yoru_TrieNode_t *)root;
    while (*key)
    {
        unsigned char index = (unsigned char)*key;
        if (!current->children[index])
        {
            Yoru_Slice_t node_slice = {0};
            Yoru_Error_t err = yoru_heap_alloc(sizeof(Yoru_TrieNode_t), &node_slice);
            if (err.type != YORU_OK)
            {
                return err;
            }
            current->children[index] = (Yoru_TrieNode_t *)node_slice.data;
            yoru_trie_init(current->children[index], root->item_size);
            current->children[index]->key = index;
        }
        current = current->children[index];
        key++;
    }

    if (current->value)
    {
        Yoru_Slice_t slice = {.data = current->value, .offset = 0, .capacity = 0};
        yoru_heap_free(&slice);
    }

    Yoru_Slice_t value_slice = {0};
    Yoru_Error_t err = yoru_heap_alloc(root->item_size, &value_slice);
    if (err.type != YORU_OK)
    {
        return err;
    }
    current->value = value_slice.data;
    current->item_size = root->item_size;
    if (current->item_size > 0)
        memset(current->value, 0, current->item_size);

    // Copy the item into the trie node
    if (current->item_size > 0)
        memcpy(current->value, item, current->item_size);
    else
        current->value = NULL; // Handle case where item_size is 0

    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
}

#endif

#endif
