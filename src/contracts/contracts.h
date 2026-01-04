#ifndef __YORU_CONTRACTS_H__
#define __YORU_CONTRACTS_H__

#include <stdio.h>
#include <assert.h>

#define Yoru_Guard_NotNull(__x)                                                                                        \
  do {                                                                                                                 \
    if (!(__x)) {                                                                                                      \
      fprintf(                                                                                                         \
          stderr,                                                                                                      \
          "%s:%d in %s: Broken Contract: %s must not be null\n",                                                       \
          (__FILE__),                                                                                                  \
          (__LINE__),                                                                                                  \
          (__func__),                                                                                                  \
          #__x);                                                                                                       \
      assert(0);                                                                                                       \
    }                                                                                                                  \
  } while (0)

#endif
