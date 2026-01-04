#ifndef __YORU_REF_H__
#define __YORU_REF_H__

#include "types.h"

/// @brief Represents a ptr that CANNOT be null. (unless youre silly! ^^)
// Please make sure to use the provided constructor `yoru_ref_make` to make a new ref
// and use `yoru_ref_get` to get the actual ptr contained in the ref struct.
// If in any way the ptr is null when creating the ref or getting the ptr it will fail
// because the `contract` is broken.
typedef struct Yoru_Ref {
  const anyptr ptr;
} Yoru_Ref;

Yoru_Ref yoru_ref_make(const anyptr ptr);
anyptr   yoru_ref_get(const Yoru_Ref *ref);

#ifdef YORU_IMPL
  #include "../contracts/contracts.h"

Yoru_Ref yoru_ref_make(const anyptr ptr) {
  Yoru_Guard_NotNull(ptr);
  return (Yoru_Ref){.ptr = ptr};
}

anyptr yoru_ref_get(const Yoru_Ref *ref) {
  Yoru_Guard_NotNull(ref);
  Yoru_Guard_NotNull(ref->ptr);
  return ref->ptr;
}

#endif
#endif
