#ifndef __YORU_BASIC_MATH_H__
#define __YORU_BASIC_MATH_H__

#include "../types/types.h"

i64 yoru_floor(f64 x);
i64 yoru_ceil(f64 x);

#ifdef YORU_IMPL
i64 yoru_floor(f64 x) {
  i64 i = (i64)x;
  return (x < 0 && x != i) ? i - 1 : i;
}

i64 yoru_ceil(f64 x) {
  i64 i = (i64)x;
  return (x > 0 && x != i) ? i + 1 : i;
}

#endif
#endif
