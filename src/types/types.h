#ifndef __YORU_TYPES_H__
#define __YORU_TYPES_H__

#include <stdint.h>
#include <stddef.h>

#define I8_MIN INT8_MIN
#define I8_MAX INT8_MAX

#define U8_MIN 0
#define U8_MAX UINT8_MAX

#define I16_MIN INT16_MIN
#define I16_MAX INT16_MAX

#define U16_MIN 0
#define U16_MAX UINT16_MAX

#define I32_MIN INT32_MIN
#define I32_MAX INT32_MAX

#define U32_MIN 0
#define U32_MAX UINT32_MAX

#define I64_MIN INT64_MIN
#define I64_MAX INT64_MAX

#define U64_MIN 0
#define U64_MAX UINT64_MAX

#define USIZE_MAX SIZE_MAX

typedef int8_t        i8;
typedef uint8_t       u8;
typedef int16_t       i16;
typedef uint16_t      u16;
typedef int32_t       i32;
typedef uint32_t      u32;
typedef int64_t       i64;
typedef uint64_t      u64;
typedef float         f32;
typedef double        f64;
typedef unsigned char byte;
typedef size_t        usize;
typedef void         *anyptr;

#define PTR_SIZE sizeof(anyptr)

#endif
