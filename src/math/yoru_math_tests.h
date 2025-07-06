#include "../asserts/yoru_asserts.h"
#include "yoru_math.h"

#include "../testing/yoru_tests.h"
#include "../collections/yoru_vector.h"
#include "../allocators/yoru_allocators.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NUM_CASES 7

#define EXPECT_MSG(msg_ptr, expected, actual) sprintf(msg_ptr, "Expected: %ld, Actual: %ld\n", (i64)expected, (i64)actual)

#define GENERATE_MODULO_TESTS(TYPE, F)                                            \
    YoruTestResult_t test_##TYPE##_modulo(void)                                   \
    {                                                                             \
        TYPE expected[NUM_CASES] = {0, 0, 0, 0, 2, 3, 3};                         \
        TYPE results[NUM_CASES] = {                                               \
            F(0, 5),                                                              \
            F(7, 7),                                                              \
            F(5, 0),                                                              \
            F(5, -3),                                                             \
            F(-3, 5),                                                             \
            F(13, 5),                                                             \
            F(3, 5)};                                                             \
        for (size_t i = 0; i < NUM_CASES; i++)                                    \
        {                                                                         \
            if (results[i] != expected[i])                                        \
            {                                                                     \
                char *msg = (char *)malloc(100);                                  \
                if (msg == NULL)                                                  \
                {                                                                 \
                    return (YoruTestResult_t){false, "Memory allocation failed"}; \
                }                                                                 \
                EXPECT_MSG(msg, expected[i], results[i]);                         \
                return (YoruTestResult_t){false, msg};                            \
            }                                                                     \
        }                                                                         \
        return (YoruTestResult_t){true, "All tests passed"};                      \
    }

GENERATE_MODULO_TESTS(i8, i8_modulo)
GENERATE_MODULO_TESTS(i16, i16_modulo)
GENERATE_MODULO_TESTS(i32, i32_modulo)
GENERATE_MODULO_TESTS(i64, i64_modulo)

#define NUM_UNSIGNED_CASES 5
#define EXPECT_MSG_UNSIGNED(msg_ptr, expected, actual) sprintf(msg_ptr, "Expected: %lu, Actual: %lu\n", (u64)expected, (u64)actual)
#define GENERATE_UNSIGNED_MODULO_TESTS(TYPE, F)                                   \
    YoruTestResult_t test_##TYPE##_modulo(void)                                   \
    {                                                                             \
        TYPE expected[NUM_UNSIGNED_CASES] = {0, 0, 0, 3, 3};                      \
        TYPE results[NUM_UNSIGNED_CASES] = {                                      \
            F(0, 5),                                                              \
            F(7, 7),                                                              \
            F(5, 0),                                                              \
            F(13, 5),                                                             \
            F(3, 5)};                                                             \
        for (size_t i = 0; i < NUM_UNSIGNED_CASES; i++)                           \
        {                                                                         \
            if (results[i] != expected[i])                                        \
            {                                                                     \
                char *msg = (char *)malloc(100);                                  \
                if (msg == NULL)                                                  \
                {                                                                 \
                    return (YoruTestResult_t){false, "Memory allocation failed"}; \
                }                                                                 \
                EXPECT_MSG_UNSIGNED(msg, expected[i], results[i]);                \
                return (YoruTestResult_t){false, msg};                            \
            }                                                                     \
        }                                                                         \
        return (YoruTestResult_t){true, "All tests passed"};                      \
    }

GENERATE_UNSIGNED_MODULO_TESTS(u8, u8_modulo)
GENERATE_UNSIGNED_MODULO_TESTS(u16, u16_modulo)
GENERATE_UNSIGNED_MODULO_TESTS(u32, u32_modulo)
GENERATE_UNSIGNED_MODULO_TESTS(u64, u64_modulo)
GENERATE_UNSIGNED_MODULO_TESTS(usize, usize_modulo)
