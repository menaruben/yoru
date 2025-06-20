#ifndef __YORU_MODULO_H__
#define __YORU_MODULO_H__

#include "../inttypes/yoru_inttypes.h"

#define modulo(a, b)          \
    do                        \
    {                         \
        if (a == 0 || a == b) \
        {                     \
            return 0;         \
        }                     \
                              \
        while (a < 0)         \
        {                     \
            a += b;           \
        }                     \
                              \
        while (a >= b)        \
        {                     \
            a -= b;           \
        }                     \
    } while (0);

static inline i8 i8_modulo(i8 a, i8 b)
{
    modulo(a, b);
    return a;
}

static inline i16 i16_modulo(i16 a, i16 b)
{
    modulo(a, b);
    return a;
}

static inline i32 i32_modulo(i32 a, i32 b)
{
    modulo(a, b);
    return a;
}

static inline i64 i64_modulo(i64 a, i64 b)
{
    modulo(a, b);
    return a;
}

static inline u8 u8_modulo(u8 a, u8 b)
{
    modulo(a, b);
    return a;
}

static inline u16 u16_modulo(u16 a, u16 b)
{
    modulo(a, b);
    return a;
}

static inline u32 u32_modulo(u32 a, u32 b)
{
    modulo(a, b);
    return a;
}

static inline u64 u64_modulo(u64 a, u64 b)
{
    modulo(a, b);
    return a;
}

static inline usize usize_modulo(usize a, usize b)
{
    modulo(a, b);
    return a;
}

#endif