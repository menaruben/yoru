#ifndef __YORU_STRING_H__
#define __YORU_STRING_H__

#include "../inttypes/yoru_inttypes.h"
#include <stdbool.h>

/**
 * @brief Represents a string with explicit length.
 */
typedef struct
{
    const u8 *str;
    const size_t length;
} String_t;

String_t String_new(const u8 *str);
const u8 *String_cstr(const String_t s, Allocator_t *allocator);
String_t String_substring(const String_t s, size_t start, size_t end);
String_t String_at(const String_t s, size_t index);
bool String_equals(const String_t *a, const String_t *b);
bool String_equals_linear(const String_t *a, const String_t *b);

String_t String_new(const u8 *str)
{
    size_t len = 0;
    while (str[len] != '\0')
    {
        len++;
    }
    return (String_t){str, len};
}

const u8 *String_cstr(const String_t s, Allocator_t *allocator)
{
    if (s.str == NULL || s.length == 0)
    {
        return NULL;
    }

    u8 *cstr = (u8 *)allocator->alloc(allocator->context, s.length + 1);
    if (cstr == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < s.length; i++)
    {
        cstr[i] = s.str[i];
    }
    cstr[s.length] = '\0'; // Null-terminate the string
    return cstr;
}

String_t String_substring(const String_t s, size_t start, size_t end)
{
    if (start >= s.length || end > s.length || start >= end)
    {
        return (String_t){NULL, 0};
    }

    return (String_t){s.str + start, end - start};
}

String_t String_at(const String_t s, size_t index)
{
    if (index >= s.length)
    {
        return (String_t){NULL, 0};
    }

    return (String_t){s.str + index, 1};
}

/**
 * @brief Compares two String_t objects for equality.
 *
 * This function checks if two String_t objects are equal by comparing their lengths
 * and the content of their strings. It returns true if both strings are equal, false otherwise.
 *
 * @param a The first String_t object to compare.
 * @param b The second String_t object to compare.
 * @return true if the strings are equal, false otherwise.
 */
bool String_equals(const String_t *a, const String_t *b)
{
    if (a->length != b->length)
        return false;

    for (size_t i = 0; i < a->length; i++)
    {
        if (a->str[i] != b->str[i])
            return false;
    }

    return true;
}

/**
 * @brief Compares two String_t objects for equality in a linear fashion.
 *
 * This function iterates over both strings up to the length of the longer string,
 * comparing each character. If the strings differ in length or content, the result
 * will indicate inequality. The comparison is performed in a way that is resistant
 * to timing attacks by always iterating over the maximum length and using bitwise
 * operations.
 *
 * @param a The first String_t object to compare.
 * @param b The second String_t object to compare.
 * @return true if the strings are equal in length and content, false otherwise.
 */
bool String_equals_linear(const String_t *a, const String_t *b)
{
    u8 result = 0;
    size_t max_length = a->length > b->length ? a->length : b->length;

    for (size_t i = 0; i < max_length; i++)
    {
        String_t sa = String_at(*a, i);
        String_t sb = String_at(*b, i);

        if (sa.str == NULL || sb.str == NULL)
        {
            result |= 1;
            continue;
        }
        result |= sa.str[0] ^ sb.str[0];
    }

    return result == 0;
}

#endif