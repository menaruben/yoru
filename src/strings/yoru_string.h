#ifndef __YORU_STRING_H__
#define __YORU_STRING_H__

#include "../inttypes/yoru_inttypes.h"
#include <stdbool.h>
#include <stdarg.h>
#include "../yoru_defs.h"

/**
 * @brief Represents a string with explicit length.
 */
typedef struct
{
    const u8 *str;
    const size_t length;
} String_t;

YORU_API String_t String_new(const char *cstr);
YORU_API const char *String_to_cstr(const String_t s, Yoru_Allocator_t *allocator);
YORU_API String_t String_format(const char *format, ...);
YORU_API String_t String_substring(const String_t s, size_t start, size_t end);
YORU_API String_t String_at(const String_t s, size_t index);
YORU_API bool String_equals(const String_t *a, const String_t *b);
YORU_API bool String_equals_linear(const String_t *a, const String_t *b);

YORU_API String_t String_new(const char *cstr)
{
    size_t len = 0;
    while (cstr[len] != '\0')
    {
        len++;
    }

    return (String_t){(const u8 *)cstr, len};
}

YORU_API const char *String_to_cstr(const String_t s, Yoru_Allocator_t *allocator)
{
    if (s.str == NULL || s.length == 0)
    {
        return NULL;
    }

    char *cstr = (char *)allocator->alloc(allocator->context, s.length + 1);
    if (cstr == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < s.length; i++)
    {
        cstr[i] = s.str[i];
    }
    cstr[s.length] = '\0'; // Null-terminate the string
    return (const char *)cstr;
}

YORU_API String_t String_format(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    size_t length = vsnprintf(NULL, 0, format, args);
    va_end(args);
    if (length < 0)
    {
        return (String_t){NULL, 0};
    }

    u8 *buffer = (u8 *)malloc(length + 1);
    if (buffer == NULL)
    {
        return (String_t){NULL, 0};
    }

    va_start(args, format);
    vsnprintf((char *)buffer, length + 1, format, args);
    va_end(args);

    return (String_t){buffer, length};
}

YORU_API String_t String_substring(const String_t s, size_t start, size_t end)
{
    if (start >= s.length || end > s.length || start >= end)
    {
        return (String_t){NULL, 0};
    }

    return (String_t){s.str + start, end - start};
}

YORU_API String_t String_at(const String_t s, size_t index)
{
    if (index >= s.length)
    {
        return (String_t){NULL, 0};
    }

    return (String_t){s.str + index, 1};
}

/**
 * @brief Compares two String_t objects for equality.
 */
YORU_API bool String_equals(const String_t *a, const String_t *b)
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
 */
YORU_API bool String_equals_linear(const String_t *a, const String_t *b)
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