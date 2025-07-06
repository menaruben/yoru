#include "../yoru.h"
#include <string.h>
#include <stdio.h>
#include "../testing/yoru_tests.h"

YoruTestResult_t test_string_new(void)
{
    const char *cstr = "hello";
    String_t s = String_new(cstr);
    if (s.length != 5)
    {
        return (YoruTestResult_t){false, "Expected length 5"};
    }
    if (memcmp(s.str, cstr, 5) != 0)
    {
        return (YoruTestResult_t){false, "String content mismatch"};
    }
    return (YoruTestResult_t){true, "test_string_new passed"};
}

YoruTestResult_t test_string_substring(void)
{
    String_t s = String_new("abcdef");
    String_t sub = String_substring(s, 1, 4);
    if (sub.length != 3 || memcmp(sub.str, "bcd", 3) != 0)
    {
        return (YoruTestResult_t){false, "Substring mismatch"};
    }
    String_t sub2 = String_substring(s, 4, 2);
    if (sub2.str != NULL || sub2.length != 0)
    {
        return (YoruTestResult_t){false, "Out of bounds substring not null/empty"};
    }
    return (YoruTestResult_t){true, "test_string_substring passed"};
}

YoruTestResult_t test_string_at(void)
{
    String_t s = String_new("xyz");
    String_t ch = String_at(s, 1);
    if (ch.length != 1 || ch.str[0] != 'y')
    {
        return (YoruTestResult_t){false, "Char at index 1 mismatch"};
    }
    String_t ch2 = String_at(s, 5);
    if (ch2.str != NULL || ch2.length != 0)
    {
        return (YoruTestResult_t){false, "Out of bounds char not null/empty"};
    }
    return (YoruTestResult_t){true, "test_string_at passed"};
}

YoruTestResult_t test_string_equals(void)
{
    String_t a = String_new("test");
    String_t b = String_new("test");
    String_t c = String_new("Test");
    String_t d = String_new("testing");
    if (!String_equals(&a, &b))
    {
        return (YoruTestResult_t){false, "a != b"};
    }
    if (String_equals(&a, &c))
    {
        return (YoruTestResult_t){false, "a == c"};
    }
    if (String_equals(&a, &d))
    {
        return (YoruTestResult_t){false, "a == d"};
    }
    return (YoruTestResult_t){true, "test_string_equals passed"};
}

YoruTestResult_t test_string_equals_linear(void)
{
    String_t a = String_new("abc123");
    String_t b = String_new("abc123");
    String_t c = String_new("abc124");
    String_t d = String_new("abc12");
    if (!String_equals_linear(&a, &b))
    {
        return (YoruTestResult_t){false, "a != b"};
    }
    if (String_equals_linear(&a, &c))
    {
        return (YoruTestResult_t){false, "a == c"};
    }
    if (String_equals_linear(&a, &d))
    {
        return (YoruTestResult_t){false, "a == d"};
    }
    return (YoruTestResult_t){true, "test_string_equals_linear passed"};
}

YoruTestResult_t test_string_format(void)
{
    String_t s = String_format("Hello %s, your score is %d", "Alice", 42);
    if (s.length == 0 || s.str == NULL)
    {
        return (YoruTestResult_t){false, "String format failed"};
    }

    const u8 expected[] = "Hello Alice, your score is 42";
    if (memcmp(s.str, expected, s.length) != 0)
    {
        return (YoruTestResult_t){false, "Formatted string mismatch"};
    }

    return (YoruTestResult_t){true, "test_string_format passed"};
}

YoruTestResult_t test_string_to_cstr(void)
{
    Allocator_t *allocator = HeapAllocator_new();
    if (allocator == NULL)
    {
        return (YoruTestResult_t){false, "Allocator creation failed"};
    }

    String_t s = String_new("hello");
    const char *cstr = String_to_cstr(s, allocator);
    if (cstr == NULL || strcmp(cstr, "hello") != 0)
    {
        return (YoruTestResult_t){false, "String to C string conversion failed"};
    }
    free((void *)cstr); // Free the allocated C string
    return (YoruTestResult_t){true, "test_string_to_cstr passed"};
}