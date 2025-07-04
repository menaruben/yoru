#include "../testing/yoru_tests.h"
#include "../collections/yoru_collections.h"

// List Tests
YoruTestResult_t test_list_basic(void)
{
    // Setup allocator
    Allocator_t *allocator = HeapAllocator_new();
    if (!allocator)
        return (YoruTestResult_t){false, "Failed to create allocator"};

    // Create list of ints
    List_t(int) list = list_new(int, allocator);

    int a = 42, b = 7, c = 99;
    list_append(allocator, list, &a);
    list_append(allocator, list, &b);
    list_prepend(allocator, list, &c);

    if (list.size != 3)
        return (YoruTestResult_t){false, "List size should be 3"};

    int *val0 = list_get(list, 0);
    int *val1 = list_get(list, 1);
    int *val2 = list_get(list, 2);

    if (!val0 || *val0 != 99)
        return (YoruTestResult_t){false, "First element should be 99"};
    if (!val1 || *val1 != 42)
        return (YoruTestResult_t){false, "Second element should be 42"};
    if (!val2 || *val2 != 7)
        return (YoruTestResult_t){false, "Third element should be 7"};

    list_remove(allocator, list, 1);
    if (list.size != 2)
        return (YoruTestResult_t){false, "List size should be 2 after remove"};

    list_destroy(list, allocator);
    return (YoruTestResult_t){true, "test_list_basic passed"};
}

// Vector Tests
YoruTestResult_t test_vector_basic(void)
{
    Allocator_t *allocator = HeapAllocator_new();
    if (!allocator)
        return (YoruTestResult_t){false, "Failed to create allocator"};

    Vec_t(int) vec = vec_new(int, allocator);

    int a = 10, b = 20, c = 30;
    vec_append(vec, a, allocator);
    vec_append(vec, b, allocator);
    vec_append(vec, c, allocator);

    if (vec.size != 3)
        return (YoruTestResult_t){false, "Vector size should be 3"};
    int *item2 = (int *)vec_get_impl(vec.items, 2, vec.size, sizeof(int));
    if (!item2 || *item2 != 30)
        return (YoruTestResult_t){false, "Third element should be 30"};

    vec_destroy(vec, allocator);
    return (YoruTestResult_t){true, "test_vector_basic passed"};
}