#define YORU_IMPLEMENTATION
#include "../yoru.h"
#include <stdio.h>

int main(void)
{
    // Allocate root node for the trie
    Yoru_TrieNode_t trie = {0};

    // Initialize trie for int values only
    Yoru_Error_t err = yoru_trie_init(&trie, sizeof(int));
    if (err.type != YORU_OK)
    {
        printf("Failed to init trie: %s\n", err.message);
        return 1;
    }

    // Store some integers
    int a = 42, b = 99;
    err = yoru_trie_set(&trie, "first", &a);
    if (err.type != YORU_OK)
    {
        printf("Error putting first: %s\n", err.message);
        yoru_trie_free(&trie);
        return 1;
    }
    err = yoru_trie_set(&trie, "second", &b);
    if (err.type != YORU_OK)
    {
        printf("Error putting second: %s\n", err.message);
        yoru_trie_free(&trie);
        return 1;
    }

    // Retrieve and print integers
    int out = 0;
    err = yoru_trie_get(&trie, "first", &out);
    if (err.type == YORU_OK)
        printf("first: %d\n", out);
    else
        printf("Error getting first: %s\n", err.message);

    err = yoru_trie_get(&trie, "second", &out);
    if (err.type == YORU_OK)
        printf("second: %d\n", out);
    else
        printf("Error getting second: %s\n", err.message);

    // Try unknown key
    err = yoru_trie_get(&trie, "unknown", &out);
    if (err.type != YORU_OK)
        printf("expected error getting unknown: %s\n", err.message);
    else
        printf("unexpected value for unknown key: %d\n", out);

    // Free trie
    yoru_trie_free(&trie);
    return 0;
}