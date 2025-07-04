#include "../yoru.h"
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>

#include "../math/yoru_math_tests.h"
#include "../strings/yoru_strings_tests.h"
#include "../allocators/yoru_allocators_tests.h"
#include "../collections/yoru_collections_tests.h"

static YoruTestFunc_t test_funcs[] = {
    // Math tests
    {.name = "test_i8_modulo", .test_func = test_i8_modulo},
    {.name = "test_i16_modulo", .test_func = test_i16_modulo},
    {.name = "test_i32_modulo", .test_func = test_i32_modulo},
    {.name = "test_i64_modulo", .test_func = test_i64_modulo},
    {.name = "test_u8_modulo", .test_func = test_u8_modulo},
    {.name = "test_u16_modulo", .test_func = test_u16_modulo},
    {.name = "test_u32_modulo", .test_func = test_u32_modulo},
    {.name = "test_u64_modulo", .test_func = test_u64_modulo},
    {.name = "test_usize_modulo", .test_func = test_usize_modulo},

    // String tests
    {.name = "test_string_new", .test_func = test_string_new},
    {.name = "test_string_substring", .test_func = test_string_substring},
    {.name = "test_string_at", .test_func = test_string_at},
    {.name = "test_string_equals", .test_func = test_string_equals},
    {.name = "test_string_equals_linear", .test_func = test_string_equals_linear},

    // Allocator tests
    {.name = "test_arena_allocator", .test_func = test_arena_allocator},
    {.name = "test_heap_allocator", .test_func = test_heap_allocator},

    // Collections tests
    {.name = "test_list_basic", .test_func = test_list_basic},
    {.name = "test_vector_basic", .test_func = test_vector_basic}};

static const size_t test_count = sizeof(test_funcs) / sizeof(test_funcs[0]);

int main(void)
{
    YoruTestSuite_t suite = {
        .tests = test_funcs,
        .test_count = test_count,
    };

    return YoruTest_run(&suite);
}