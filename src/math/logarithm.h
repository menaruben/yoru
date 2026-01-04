#ifndef __YORU_LOGARITHM_H__
#define __YORU_LOGARITHM_H__

#include "../types/types.h"

f64 yoru_log(f64 x, f64 base);
f64 yoru_ln(f64 x);

#ifdef YORU_IMPL
  #include "constants.h"
  #include <assert.h>

f64 yoru_log(f64 x, f64 base) {
  assert(base != 0.0);
  assert(base != 1.0);
  return yoru_ln(x) / yoru_ln(base);
}

// see: https://en.wikipedia.org/wiki/Taylor_series
// ln(x) ~= (1/1) * (x-1)^1 - (1/2) * (x-1)^2 + (1/3) * (x-1)^3
// TODO: fast enough for now, but need to optimize...
f64 yoru_ln(f64 x) {
  assert(x > 0);
  if (x == 1.0) return 0.0;
  if (x == YORU_EULER) return 1.0;
  if (x == 2) return YORU_LN2;

  // Since the taylor series is good for values between 1 and 2 we need
  // to break down the input value x into multiple part because:
  // ln(50) = ln(2) + ln(25)
  //        = ln(2) + ln(2) + ln(12.5)
  //        = ln(2) + ln(2) + ln(2) + ln(6.25)
  //        = ln(2) + ln(2) + ln(2) + ln(2) + ln(3.125)
  //        = ln(2) + ln(2) + ln(2) + ln(2) + ln(2) + ln(1.5625)
  //        = 5 * ln(2) + ln(1.5625)

  i64 ln2_count = 0;
  while (x > 2) {
    x /= 2;
    ln2_count++;
  }

  while (x < 1) {
    x *= 2;
    ln2_count--;
  }

  // here x should now be in the range (1, 2)
  f64   res      = ln2_count * YORU_LN2;
  i8    sign     = 1;
  f64   term     = x - 1;
  usize max_iter = 100; // TODO: could add tolerance to stop earlier
  for (usize i = 1; i < max_iter; ++i) {
    f64 t = sign * 1.0 / i;
    for (usize n = 1; n <= i; ++n)
      t *= term;

    res += t;
    sign *= -1;
  }

  return res;
}

#endif
#endif
