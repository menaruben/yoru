#ifndef __YORU_TRIE_H__
#define __YORU_TRIE_H__

/*
    A Trie is a tree-like data structure to store
    key value pairs, where the keys are usually strings.

    Each node in the Trie represents a single character/integer.
    For example, if you would want to get the value for the key "mykey",
    you would start at the root node, then go to the child node
    that has the key 'm', then to its child node with key 'y', and so on
    until you reach your null-terminator or the end of the key.
    The reached node will then contain the value for the key "mykey".

    For more information, see:
    https://en.wikipedia.org/wiki/Trie
*/

// TODO: Implement

#include <stddef.h>
#include "../inttypes/yoru_inttypes.h"
#include "../allocators/yoru_allocators.h"

#define TrieNode_t(TValue)                 \
    struct TrieNode_##T                    \
    {                                      \
        u8 key;                            \
        struct TrieNode_##T children[256]; \
        TValue *value;                     \
    }

#define Trie_t(TValue)                  \
    struct                              \
    {                                   \
        struct TrieNode_##TValue *root; \
    }

static inline void trie_init(Allocator_t *allocator);

#endif