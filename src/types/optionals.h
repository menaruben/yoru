#ifndef __YORU_OPTIONALS_H__
#define __YORU_OPTIONALS_H__

#include "types.h"

#include <stdbool.h>

/// @brief Yoru_Opt is a struct that holds a pointer that might be NULL
// Check `has_value` before trying to deref ptr
typedef struct Yoru_Opt {
  const anyptr ptr;
  bool         has_value;
} Yoru_Opt;

Yoru_Opt yoru_opt_some(const anyptr ptr);
Yoru_Opt yoru_opt_none();

#ifdef YORU_IMPL
  #include "../contracts/contracts.h"

Yoru_Opt yoru_opt_some(const anyptr ptr) {
  Yoru_Guard_NotNull(ptr);
  return (Yoru_Opt){.ptr = ptr, .has_value = true};
}

Yoru_Opt yoru_opt_none() {
  return (Yoru_Opt){.ptr = nullptr, .has_value = false};
}

#endif
#endif
