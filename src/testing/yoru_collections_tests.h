#include "yoru_tests.h"
#include "../collections/yoru_collections.h"

// List Tests
Yoru_TestResult_t test_list_basic(void)
{
    // Setup allocator
    Yoru_Allocator_t *allocator = Yoru_HeapAllocator_new();
    if (!allocator)
        return (Yoru_TestResult_t){false, "Failed to create allocator"};

    // Create list of ints
    List_t(int) list = yoru_list_new(int, allocator);

    int a = 42, b = 7, c = 99;
    yoru_list_append(allocator, list, &a);
    yoru_list_append(allocator, list, &b);
    yoru_list_prepend(allocator, list, &c);

    if (list.size != 3)
        return (Yoru_TestResult_t){false, "List size should be 3"};

    int *val0 = yoru_list_get(list, 0);
    int *val1 = yoru_list_get(list, 1);
    int *val2 = yoru_list_get(list, 2);

    if (!val0 || *val0 != 99)
        return (Yoru_TestResult_t){false, "First element should be 99"};
    if (!val1 || *val1 != 42)
        return (Yoru_TestResult_t){false, "Second element should be 42"};
    if (!val2 || *val2 != 7)
        return (Yoru_TestResult_t){false, "Third element should be 7"};

    yoru_list_remove(allocator, list, 1);
    if (list.size != 2)
        return (Yoru_TestResult_t){false, "List size should be 2 after remove"};

    yoru_list_destroy(list, allocator);
    return (Yoru_TestResult_t){true, "test_list_basic passed"};
}

// Vector Tests
Yoru_TestResult_t test_vector_basic(void)
{
    Yoru_Allocator_t *allocator = Yoru_HeapAllocator_new();
    if (!allocator)
        return (Yoru_TestResult_t){false, "Failed to create allocator"};

    Vec_t(int) vec = vec_new(int, allocator);

    int a = 10, b = 20, c = 30;
    vec_append(vec, a, allocator);
    vec_append(vec, b, allocator);
    vec_append(vec, c, allocator);

    if (vec.size != 3)
        return (Yoru_TestResult_t){false, "Vector size should be 3"};

    int *item2 = (int *)vec_get(vec, 2);
    if (!item2 || *item2 != 30)
        return (Yoru_TestResult_t){false, "Third element should be 30"};

    int new_item = 69;
    vec_set(vec, 2, new_item);
    int *retrieved = vec_get(vec, 2);

    if (*retrieved != new_item)
        return (Yoru_TestResult_t){false, "Third element should be 69 after set"};

    vec_destroy(vec, allocator);
    return (Yoru_TestResult_t){true, "test_vector_basic passed"};
}

// Trie Tests
Yoru_TestResult_t test_trie_basic(void)
{
    Yoru_Allocator_t *allocator = Yoru_HeapAllocator_new();
    if (!allocator)
        return (Yoru_TestResult_t){false, "Failed to create allocator"};

    TrieNode_t *trie = trie_new(allocator);
    if (!trie)
        return (Yoru_TestResult_t){false, "Failed to create trie"};

    struct
    {
        u8 pid[4];
        u8 *name;
        i32 age;
        struct
        {
            u8 *country;
            u8 *city;
        } address;
    } person = {
        .pid = {0x01, 0x02, 0x03, 0x04},
        .name = (u8 *)"Yoru",
        .age = 18,
        .address = {
            .country = (u8 *)"Japan",
            .city = (u8 *)"Tokyo"}};

    trie_put(trie, person.pid, &person, allocator);
    struct
    {
        u8 pid[4];
        u8 *name;
        i32 age;
        struct
        {
            u8 *country;
            u8 *city;
        } address;
    } *retrieved = trie_get(trie, person.pid);

    if (!retrieved)
        return (Yoru_TestResult_t){false, "Failed to retrieve person from trie"};

    if (memcmp(retrieved, &person, sizeof(person)))
        return (Yoru_TestResult_t){false, "Retrieved person does not match original"};

    trie_destroy(trie, allocator);
    return (Yoru_TestResult_t){true, "test_trie_basic passed"};
}