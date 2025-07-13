#define YORU_IMPLEMENTATION
#include "../yoru.h"
#include <stdio.h>

typedef struct
{
    int x, y;
} Point;

int main(void)
{
    Yoru_Allocator_t *allocator = Yoru_HeapAllocator_new();
    YORU_ASSERT_NOT_NULL(allocator);

    // A Trie can store any type of data, it is not fixed to a specific type.
    TrieNode_t *trie = trie_new(allocator);

    // Store an int
    i32 int_val = 72;
    trie_put(trie, "int_key", &int_val, allocator);

    // Store a Point
    Point pt = {10, 20};
    trie_put(trie, "point_key", &pt, allocator);

    // Retrieve and print int (auto casts to lhs type)
    i32 *retrieved_int = trie_get(i32 *, trie, "int_key");
    YORU_ASSERT_NOT_NULL(retrieved_int);
    printf("Retrieved int: %d\n", *retrieved_int);

    // get and explicit cast to specific type
    u8 *retrieved_int_as_u8 = trie_get_as(u8, trie, "int_key");
    YORU_ASSERT_NOT_NULL(retrieved_int_as_u8);
    printf("Retrieved int as u8: %c\n", *retrieved_int_as_u8);

    // Retrieve and print Point
    Point *retrieved_pt = trie_get(Point *, trie, "point_key");
    YORU_ASSERT_NOT_NULL(retrieved_pt);
    printf("Retrieved point: (%d, %d)\n", retrieved_pt->x, retrieved_pt->y);

    trie_destroy(trie, allocator);
}