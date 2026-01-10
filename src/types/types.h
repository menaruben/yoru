#ifndef __YORU_TYPES_H__
#define __YORU_TYPES_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

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
typedef u8            b8;
typedef u16           b16;
typedef u32           b32;
typedef u64           b64;
typedef unsigned char byte;
typedef size_t        usize;
typedef void         *anyptr;
typedef char         *cstr;

typedef union Yoru_Any {
  i8     i8;
  u8     u8;
  i16    i16;
  u16    u16;
  i32    i32;
  u32    u32;
  i64    i64;
  u64    u64;
  f32    f32;
  f64    f64;
  b8     b8;
  b16    b16;
  b32    b32;
  b64    b64;
  byte   byte;
  usize  usize;
  anyptr anyptr;
  cstr   cstr;
} Yoru_Any;

#define PTR_SIZE sizeof(anyptr)

#endif
