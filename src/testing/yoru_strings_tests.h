#include "../strings/yoru_strings.h"
#include <string.h>
#include <stdio.h>
#include "yoru_tests.h"

Yoru_TestResult_t test_string_new(void)
{
    const char *cstr = "hello";
    Yoru_String_t s = Yoru_String_new(cstr);
    if (s.length != 5)
    {
        return (Yoru_TestResult_t){false, "Expected length 5"};
    }
    if (memcmp(s.str, cstr, 5) != 0)
    {
        return (Yoru_TestResult_t){false, "String content mismatch"};
    }
    return (Yoru_TestResult_t){true, "test_string_new passed"};
}

Yoru_TestResult_t test_string_substring(void)
{
    Yoru_String_t s = Yoru_String_new("abcdef");
    Yoru_String_t sub = Yoru_String_substring(s, 1, 4);
    if (sub.length != 3 || memcmp(sub.str, "bcd", 3) != 0)
    {
        return (Yoru_TestResult_t){false, "Substring mismatch"};
    }
    Yoru_String_t sub2 = Yoru_String_substring(s, 4, 2);
    if (sub2.str != NULL || sub2.length != 0)
    {
        return (Yoru_TestResult_t){false, "Out of bounds substring not null/empty"};
    }
    return (Yoru_TestResult_t){true, "test_string_substring passed"};
}

Yoru_TestResult_t test_string_at(void)
{
    Yoru_String_t s = Yoru_String_new("xyz");
    Yoru_String_t ch = Yoru_String_at(s, 1);
    if (ch.length != 1 || ch.str[0] != 'y')
    {
        return (Yoru_TestResult_t){false, "Char at index 1 mismatch"};
    }
    Yoru_String_t ch2 = Yoru_String_at(s, 5);
    if (ch2.str != NULL || ch2.length != 0)
    {
        return (Yoru_TestResult_t){false, "Out of bounds char not null/empty"};
    }
    return (Yoru_TestResult_t){true, "test_string_at passed"};
}

Yoru_TestResult_t test_string_equals(void)
{
    Yoru_String_t a = Yoru_String_new("test");
    Yoru_String_t b = Yoru_String_new("test");
    Yoru_String_t c = Yoru_String_new("Test");
    Yoru_String_t d = Yoru_String_new("testing");
    if (!Yoru_String_equals(&a, &b))
    {
        return (Yoru_TestResult_t){false, "a != b"};
    }
    if (Yoru_String_equals(&a, &c))
    {
        return (Yoru_TestResult_t){false, "a == c"};
    }
    if (Yoru_String_equals(&a, &d))
    {
        return (Yoru_TestResult_t){false, "a == d"};
    }
    return (Yoru_TestResult_t){true, "test_string_equals passed"};
}

Yoru_TestResult_t test_string_equals_linear(void)
{
    Yoru_String_t a = Yoru_String_new("abc123");
    Yoru_String_t b = Yoru_String_new("abc123");
    Yoru_String_t c = Yoru_String_new("abc124");
    Yoru_String_t d = Yoru_String_new("abc12");
    if (!Yoru_String_equals_linear(&a, &b))
    {
        return (Yoru_TestResult_t){false, "a != b"};
    }
    if (Yoru_String_equals_linear(&a, &c))
    {
        return (Yoru_TestResult_t){false, "a == c"};
    }
    if (Yoru_String_equals_linear(&a, &d))
    {
        return (Yoru_TestResult_t){false, "a == d"};
    }
    return (Yoru_TestResult_t){true, "test_string_equals_linear passed"};
}

Yoru_TestResult_t test_string_format(void)
{
    Yoru_String_t s = Yoru_String_format("Hello %s, your score is %d", "Alice", 42);
    if (s.length == 0 || s.str == NULL)
    {
        return (Yoru_TestResult_t){false, "String format failed"};
    }

    const u8 expected[] = "Hello Alice, your score is 42";
    if (memcmp(s.str, expected, s.length) != 0)
    {
        return (Yoru_TestResult_t){false, "Formatted string mismatch"};
    }

    return (Yoru_TestResult_t){true, "test_string_format passed"};
}

Yoru_TestResult_t test_string_to_cstr(void)
{
    Yoru_Allocator_t *allocator = Yoru_HeapAllocator_new();
    if (allocator == NULL)
    {
        return (Yoru_TestResult_t){false, "Allocator creation failed"};
    }

    Yoru_String_t s = Yoru_String_new("hello");
    const char *cstr = Yoru_String_to_cstr(s, allocator);
    if (cstr == NULL || strcmp(cstr, "hello") != 0)
    {
        return (Yoru_TestResult_t){false, "String to C string conversion failed"};
    }
    free((void *)cstr); // Free the allocated C string
    return (Yoru_TestResult_t){true, "test_string_to_cstr passed"};
}