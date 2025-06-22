#include "../asserts/yoru_asserts.h"
#include "yoru_math.h"

#include <stdio.h>
#include <stdlib.h>

#define INTEGER_MODULO_TEST(type)               \
    do                                          \
    {                                           \
        ASSERT_EQUAL(type##_modulo(5, 3), 2);   \
        ASSERT_EQUAL(type##_modulo(-5, 3), 1);  \
        ASSERT_EQUAL(type##_modulo(5, -3), 0);  \
        ASSERT_EQUAL(type##_modulo(-5, -3), 0); \
        ASSERT_EQUAL(type##_modulo(0, 3), 0);   \
        ASSERT_EQUAL(type##_modulo(5, 0), 0);   \
    } while (0)

#define UINTEGER_MODULO_TEST(type)            \
    do                                        \
    {                                         \
        ASSERT_EQUAL(type##_modulo(5, 3), 2); \
        ASSERT_EQUAL(type##_modulo(0, 3), 0); \
        ASSERT_EQUAL(type##_modulo(5, 0), 0); \
    } while (0)

void test_i8_modulo() { INTEGER_MODULO_TEST(i8); }
void test_i16_modulo() { INTEGER_MODULO_TEST(i16); }
void test_i32_modulo() { INTEGER_MODULO_TEST(i32); }
void test_i64_modulo() { INTEGER_MODULO_TEST(i64); }

void test_u8_modulo() { UINTEGER_MODULO_TEST(u8); }
void test_u16_modulo() { UINTEGER_MODULO_TEST(u16); }
void test_u32_modulo() { UINTEGER_MODULO_TEST(u32); }
void test_u64_modulo() { UINTEGER_MODULO_TEST(u64); }
void test_usize_modulo() { UINTEGER_MODULO_TEST(usize); }

int main()
{
    test_i8_modulo();
    test_i16_modulo();
    test_i32_modulo();
    test_i64_modulo();
    test_u8_modulo();
    test_u16_modulo();
    test_u32_modulo();
    test_u64_modulo();

    printf("All tests passed.\n");
    return 0;
}