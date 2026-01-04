#ifndef __YORU_CONTRACTS_H__
#define __YORU_CONTRACTS_H__

#include <stdio.h>
#include <assert.h>

#define Yoru_Contract_Assert(cond, msg)                                                                                \
  do {                                                                                                                 \
    if (!(cond)) {                                                                                                     \
      fprintf(stderr, "%s:%d in %s: Broken Contract: %s\n", (__FILE__), (__LINE__), (__func__), (msg));                \
      assert(0);                                                                                                       \
    }                                                                                                                  \
  } while (0)

#define Yoru_Contract_Assert_f(cond, msg, _file, _line, _func)                                                         \
  do {                                                                                                                 \
    if (!(cond)) {                                                                                                     \
      fprintf(stderr, "%s:%d in %s: Broken Contract: %s\n", (_file), (_line), (_func), (msg));                         \
      assert(0);                                                                                                       \
    }                                                                                                                  \
  } while (0)

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
