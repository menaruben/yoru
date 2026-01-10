#ifndef __YORU_H__
#define __YORU_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#ifdef __linux__
#  include <sys/mman.h>
#  include <unistd.h>
#endif

/* ============================================================
   MODULE: Types
   ============================================================ */

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

#define PTR_SIZE sizeof(void *)

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

/* ============================================================
   MODULE: Optionals
   ============================================================ */

/// @brief Yoru_Opt is a struct that holds a pointer that might be NULL
//- Check `has_value` before trying to deref ptr
typedef struct Yoru_Opt {
  const anyptr ptr;
  bool         has_value;
} Yoru_Opt;

Yoru_Opt yoru_opt_some(const anyptr ptr);
Yoru_Opt yoru_opt_none();

#ifdef YORU_IMPL
Yoru_Opt yoru_opt_some(const anyptr ptr) {
  assert(ptr);
  return (Yoru_Opt){.ptr = ptr, .has_value = true};
}

Yoru_Opt yoru_opt_none() {
  return (Yoru_Opt){.ptr = NULL, .has_value = false};
}
#endif // YORU_IMPL

/* ============================================================
   MODULE: References
   ============================================================ */

/// @brief Represents a ptr that CANNOT be null. (unless youre silly! ^^)
/// Please make sure to use the provided constructor `yoru_ref_make` to make a new ref
/// and use `yoru_ref_get` to get the actual ptr contained in the ref struct.
/// If in any way the ptr is null when creating the ref or getting the ptr it will fail
/// because the `contract` is broken.
typedef struct Yoru_Ref {
  const anyptr ptr;
} Yoru_Ref;

Yoru_Ref yoru_ref_make(const anyptr ptr);
anyptr   yoru_ref_get(const Yoru_Ref *ref);

#ifdef YORU_IMPL
Yoru_Ref yoru_ref_make(const anyptr ptr) {
  assert(ptr);
  return (Yoru_Ref){.ptr = ptr};
}

anyptr yoru_ref_get(const Yoru_Ref *ref) {
  assert(ref);
  assert(ref->ptr);
  return ref->ptr;
}
#endif // YORU_IMPL

/* ============================================================
   MODULE: Allocators
   ============================================================ */

typedef Yoru_Opt (*Yoru_Allocator_Alloc_Func)(anyptr ctx, usize size);
typedef void (*Yoru_Allocator_DeAlloc_Func)(anyptr ctx, anyptr ptr);
typedef Yoru_Opt (*Yoru_Allocator_ReAlloc_Func)(anyptr ctx, usize old_size, anyptr old_ptr, usize new_size);
typedef void (*Yoru_Allocator_Destroy_Func)(anyptr ctx);

typedef struct Yoru_AllocatorVTable {
  Yoru_Allocator_Alloc_Func   alloc;
  Yoru_Allocator_DeAlloc_Func dealloc;
  Yoru_Allocator_ReAlloc_Func realloc;
  Yoru_Allocator_Destroy_Func destroy;
} Yoru_AllocatorVTable;

typedef struct Yoru_Allocator {
  const Yoru_AllocatorVTable *vtable;
  anyptr                      ctx;
} Yoru_Allocator;

Yoru_Opt yoru_allocator_alloc(Yoru_Allocator *allocator, usize size);
void     yoru_allocator_dealloc(Yoru_Allocator *allocator, anyptr ptr);
Yoru_Opt yoru_allocator_realloc(Yoru_Allocator *allocator, usize old_size, anyptr old_ptr, usize new_size);
void     yoru_allocator_destroy(Yoru_Allocator *allocator);

#ifdef YORU_IMPL
Yoru_Opt yoru_allocator_alloc(Yoru_Allocator *allocator, usize size) {
  assert(allocator);
  assert(allocator->vtable);
  assert(allocator->vtable->alloc);
  return allocator->vtable->alloc(allocator->ctx, size);
}

void yoru_allocator_dealloc(Yoru_Allocator *allocator, anyptr ptr) {
  assert(allocator);
  assert(allocator->vtable);
  assert(allocator->vtable->dealloc);
  allocator->vtable->dealloc(allocator->ctx, ptr);
}

Yoru_Opt yoru_allocator_realloc(Yoru_Allocator *allocator, usize old_size, anyptr old_ptr, usize new_size) {
  assert(allocator);
  assert(allocator->vtable);
  assert(allocator->vtable->realloc);
  return allocator->vtable->realloc(allocator->ctx, old_size, old_ptr, new_size);
}

void yoru_allocator_destroy(Yoru_Allocator *allocator) {
  assert(allocator);
  assert(allocator->vtable);
  assert(allocator->vtable->destroy);
  allocator->vtable->destroy(allocator->ctx);
}
#endif // YORU_IMPL

/* ============================================================
   MODULE: GlobalAllocator
   ============================================================ */

typedef Yoru_Allocator Yoru_GlobalAllocator;
Yoru_GlobalAllocator   yoru_global_allocator_make();

#ifdef YORU_IMPL
Yoru_Opt __yoru_global_allocator_alloc(anyptr ctx, usize size);
void     __yoru_global_allocator_dealloc(anyptr ctx, anyptr ptr);
Yoru_Opt __yoru_global_allocator_realloc(anyptr ctx, usize old_size, anyptr old_ptr, usize new_size);
void     __yoru_global_allocator_destroy(anyptr ctx);

static const Yoru_AllocatorVTable __yoru_global_allocator_vtable = {
    .alloc   = __yoru_global_allocator_alloc,
    .dealloc = __yoru_global_allocator_dealloc,
    .realloc = __yoru_global_allocator_realloc,
    .destroy = __yoru_global_allocator_destroy,
};

Yoru_GlobalAllocator yoru_global_allocator_make() {
  return (Yoru_GlobalAllocator){
      .vtable = &__yoru_global_allocator_vtable,
      .ctx    = NULL,
  };
}

Yoru_Opt __yoru_global_allocator_alloc(anyptr ctx, usize size) {
  (void)ctx;
  anyptr ptr = calloc(1, size);
  if (!ptr) { return yoru_opt_none(); }
  return yoru_opt_some(ptr);
}

void __yoru_global_allocator_dealloc(anyptr ctx, anyptr ptr) {
  (void)ctx;
  if (!ptr) return;
  free(ptr);
}

Yoru_Opt __yoru_global_allocator_realloc(anyptr ctx, usize old_size, anyptr old_ptr, usize new_size) {
  (void)ctx;
  usize  copy_size = old_size < new_size ? old_size : new_size;
  anyptr new_ptr   = calloc(1, new_size);
  if (!new_ptr) return yoru_opt_none();
  memcpy(new_ptr, old_ptr, copy_size);
  return yoru_opt_some(new_ptr);
}

void __yoru_global_allocator_destroy(anyptr ctx) {
  (void)ctx;
}
#endif // YORU_IMPL

/* ============================================================
   MODULE: ArenaAllocator
   ============================================================ */

typedef Yoru_Allocator Yoru_ArenaAllocator;
Yoru_ArenaAllocator   *yoru_arena_allocator_make(usize capacity);

#ifdef YORU_IMPL
Yoru_Opt __yoru_arena_allocator_alloc(anyptr ctx, usize size);
void     __yoru_arena_allocator_dealloc(anyptr ctx, anyptr ptr);
Yoru_Opt __yoru_arena_allocator_realloc(anyptr ctx, usize old_size, anyptr old_ptr, usize new_size);
void     __yoru_arena_allocator_destroy(anyptr ctx);

static const Yoru_AllocatorVTable __yoru_arena_allocator_vtable = {
    .alloc   = __yoru_arena_allocator_alloc,
    .dealloc = __yoru_arena_allocator_dealloc,
    .realloc = __yoru_arena_allocator_realloc,
    .destroy = __yoru_arena_allocator_destroy,
};

typedef struct Yoru_ArenaAllocatorCtx {
  byte *mem;
  usize offset;
  usize capacity;
} Yoru_ArenaAllocatorCtx;

Yoru_ArenaAllocator *yoru_arena_allocator_make(usize capacity) {
  Yoru_ArenaAllocator    *a   = NULL;
  Yoru_ArenaAllocatorCtx *ctx = NULL;
  byte                   *mem = NULL;

  a = calloc(1, sizeof(Yoru_ArenaAllocator));
  if (!a) goto yoru_arena_allocator_make_err;
  a->vtable = &__yoru_arena_allocator_vtable;

  ctx = calloc(1, sizeof(Yoru_ArenaAllocatorCtx));
  if (!ctx) goto yoru_arena_allocator_make_err;
  a->ctx = (anyptr)ctx;

  mem = (byte *)calloc(1, capacity);
  if (!mem) goto yoru_arena_allocator_make_err;
  ctx->mem      = mem;
  ctx->offset   = 0;
  ctx->capacity = capacity;
  return a;

yoru_arena_allocator_make_err:
  if (a) free(a);
  if (ctx) free(ctx);
  if (mem) free(mem);

  return NULL;
}

Yoru_Opt __yoru_arena_allocator_alloc(anyptr ctx, usize size) {
  if (!ctx) return yoru_opt_none();
  Yoru_ArenaAllocatorCtx *arena = (Yoru_ArenaAllocatorCtx *)ctx;

  if (arena->offset + size > arena->capacity) { return yoru_opt_none(); }

  anyptr ptr = arena->mem + arena->offset;
  arena->offset += size;

  return yoru_opt_some(ptr);
}

void __yoru_arena_allocator_dealloc(anyptr ctx, anyptr ptr) {
  (void)ctx;
  (void)ptr;
  /* Freeing in an arena is a no-op because the idea is that you free the entire arena once.
     For that you would call the yoru_allocator_destroy func on the arena allocator */
}

Yoru_Opt __yoru_arena_allocator_realloc(anyptr ctx, usize old_size, anyptr old_ptr, usize new_size) {
  (void)ctx;
  (void)old_size;
  (void)old_ptr;
  (void)new_size;
  /* Reallocations are not supported on arena allocators because if you want to grow something in the
     middle of the arena then we would also have to shift every ptr to the right which might not work
     becuase the arena might not be able to fit everything with the new ptr in the arena and we dont
     know in the arena ctx where which ptr starts! otherwise we would have to keep track of them here.

     -> maybe we could handle reallocs like normal allocations instead and just push to the arena?
  */
  return yoru_opt_none();
}

void __yoru_arena_allocator_destroy(anyptr ctx) {
  if (!ctx) return;
  Yoru_ArenaAllocatorCtx *c = (Yoru_ArenaAllocatorCtx *)ctx;
  if (c->mem) free(c->mem);
  c->mem      = NULL;
  c->offset   = 0;
  c->capacity = 0;
}
#endif // YORU_IMPL

/* ============================================================
   MODULE: VirtualMemory
   ============================================================ */

typedef struct Yoru_Vmem_Ctx {
  anyptr base;
  usize  commit_pos;
  usize  addr_space_size;
} Yoru_Vmem_Ctx;

usize yoru_align_up(usize x, usize alignment);
usize yoru_get_page_size();
bool  yoru_vmem_reserve(usize size, Yoru_Vmem_Ctx *out_ctx);
bool  yoru_vmem_commit(Yoru_Vmem_Ctx *ctx, usize size);
bool  yoru_vmem_free(Yoru_Vmem_Ctx *ctx);

#ifdef YORU_IMPL
usize yoru_align_up(usize x, usize alignment) {
  return (x + alignment - 1) & ~(alignment - 1);
}

usize yoru_get_page_size() {
#  ifdef __linux__
  return (usize)sysconf(_SC_PAGE_SIZE);
#  else
#    error "platform not supported yet"
#  endif
}

static inline bool __yoru_vmem_reserve_linux(usize size, Yoru_Vmem_Ctx *ctx);

bool yoru_vmem_reserve(usize size, Yoru_Vmem_Ctx *out_ctx) {
  assert(out_ctx && "must not be null");

#  ifdef __linux__
  return __yoru_vmem_reserve_linux(size, out_ctx);
#  else
#    error "platform not supported yet"
#  endif
}

bool yoru_vmem_commit(Yoru_Vmem_Ctx *ctx, usize size) {
  assert(ctx && "must not be null");
  assert(ctx->base && "must not be null");
#  ifdef __linux__
  usize size_aligned = yoru_align_up(size, yoru_get_page_size());
  int   err          = mprotect((u8 *)ctx->base + ctx->commit_pos, size_aligned, PROT_READ | PROT_WRITE);
  if (err != 0) return false;
  ctx->commit_pos += size_aligned;
  return true;
#  else
#    error "platform not supported yet"
#  endif
}

static inline bool __yoru_vmem_free_linux(Yoru_Vmem_Ctx *ctx);

bool yoru_vmem_free(Yoru_Vmem_Ctx *ctx) {
  assert(ctx && "must not be null");
  assert(ctx->base && "must not be null");
#  ifdef __linux
  return __yoru_vmem_free_linux(ctx);
#  else
#    error "platform not supported yet"
#  endif
}

#  ifdef __linux__
static inline bool __yoru_vmem_reserve_linux(usize size, Yoru_Vmem_Ctx *ctx) {
  anyptr ptr = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
  if (ptr == MAP_FAILED) {
    ctx->base;
    ctx->commit_pos      = 0;
    ctx->addr_space_size = 0;
    return false;
  }

  ctx->base            = ptr;
  ctx->commit_pos      = 0;
  ctx->addr_space_size = size;
  return true;
}

static inline bool __yoru_vmem_free_linux(Yoru_Vmem_Ctx *ctx) {
  int err = munmap(ctx->base, ctx->addr_space_size);
  if (err != 0) return false;
  ctx->base            = NULL;
  ctx->commit_pos      = 0;
  ctx->addr_space_size = 0;
  return true;
}
#  endif

#endif // YORU_IMPL

/* ============================================================
   MODULE: VirtualArenaAllocator
   ============================================================ */

typedef Yoru_Allocator      Yoru_VirtualArenaAllocator;
Yoru_VirtualArenaAllocator *yoru_virtual_arena_allocator_make(usize capacity);

#ifdef YORU_IMPL
Yoru_Opt __yoru_virtual_arena_allocator_alloc(anyptr ctx, usize size);
void     __yoru_virtual_arena_allocator_dealloc(anyptr ctx, anyptr ptr);
Yoru_Opt __yoru_virtual_arena_allocator_realloc(anyptr ctx, usize old_size, anyptr old_ptr, usize new_size);
void     __yoru_virtual_arena_allocator_destroy(anyptr ctx);

static const Yoru_AllocatorVTable __yoru_virtual_arena_allocator_vtable = {
    .alloc   = __yoru_virtual_arena_allocator_alloc,
    .dealloc = __yoru_virtual_arena_allocator_dealloc,
    .realloc = __yoru_virtual_arena_allocator_realloc,
    .destroy = __yoru_virtual_arena_allocator_destroy,
};

typedef struct Yoru_VirtualArenaAllocatorCtx {
  usize          offset;
  Yoru_Vmem_Ctx *vmem_ctx;
} Yoru_VirtualArenaAllocatorCtx;

Yoru_ArenaAllocator *yoru_virtual_arena_allocator_make(usize capacity) {
  Yoru_ArenaAllocator *a = calloc(1, sizeof *a);
  if (!a) return NULL;

  Yoru_VirtualArenaAllocatorCtx *ctx = calloc(1, sizeof *ctx);
  if (!ctx) goto err;

  Yoru_Vmem_Ctx *vmem_ctx = calloc(1, sizeof *vmem_ctx);
  if (!vmem_ctx) goto err;

  capacity = yoru_align_up(capacity, yoru_get_page_size());

  if (!yoru_vmem_reserve(capacity, vmem_ctx)) goto err;
  if (!yoru_vmem_commit(vmem_ctx, yoru_get_page_size())) goto err;

  ctx->offset   = 0;
  ctx->vmem_ctx = vmem_ctx;

  a->vtable = &__yoru_virtual_arena_allocator_vtable;
  a->ctx    = ctx;
  return a;

err:
  if (vmem_ctx) {
    yoru_vmem_free(vmem_ctx);
    free(vmem_ctx);
  }
  free(ctx);
  free(a);
  return NULL;
}

Yoru_Opt __yoru_virtual_arena_allocator_alloc(anyptr ctx, usize size) {
  if (!ctx) return yoru_opt_none();

  Yoru_VirtualArenaAllocatorCtx *arena = ctx;
  Yoru_Vmem_Ctx                 *vm    = arena->vmem_ctx;
  if (arena->offset + size > vm->addr_space_size) return yoru_opt_none();

  usize needed = arena->offset + size;
  usize page   = yoru_get_page_size();

  while (vm->commit_pos < needed) {
    if (!yoru_vmem_commit(vm, page)) return yoru_opt_none();
  }

  anyptr ptr = (char *)vm->base + arena->offset;
  arena->offset += size;

  return yoru_opt_some(ptr);
}

void __yoru_virtual_arena_allocator_dealloc(anyptr ctx, anyptr ptr) {
  (void)ctx;
  (void)ptr;
  /* Freeing in an arena is a no-op because the idea is that you free the entire arena once.
     For that you would call the yoru_allocator_destroy func on the arena allocator */
}

Yoru_Opt __yoru_virtual_arena_allocator_realloc(anyptr ctx, usize old_size, anyptr old_ptr, usize new_size) {
  (void)ctx;
  (void)old_size;
  (void)old_ptr;
  (void)new_size;
  /* Reallocations are not supported on arena allocators because if you want to grow something in the
     middle of the arena then we would also have to shift every ptr to the right which might not work
     becuase the arena might not be able to fit everything with the new ptr in the arena and we dont
     know in the arena ctx where which ptr starts! otherwise we would have to keep track of them here.

     -> maybe we could handle reallocs like normal allocations instead and just push to the arena?
  */
  return yoru_opt_none();
}

void __yoru_virtual_arena_allocator_destroy(anyptr ctx) {
  assert(ctx && "must not be null");
  Yoru_VirtualArenaAllocatorCtx *c = (Yoru_VirtualArenaAllocatorCtx *)ctx;
  assert(c->vmem_ctx && "must not be null");

  yoru_vmem_free(c->vmem_ctx);
  free(c->vmem_ctx);
  free(c);
  c->offset                    = 0;
  c->vmem_ctx->base            = NULL;
  c->vmem_ctx->commit_pos      = 0;
  c->vmem_ctx->addr_space_size = 0;
}
#endif // YORU_IMPL

#define YORU_B(_n) ((usize)1 * (_n))
#define YORU_KB(_n) ((_n) * YORU_B(1000))    // 1 KB = 1000 bytes
#define YORU_KiB(_n) ((_n) * YORU_B(1024))   // 1 KiB = 1024 bytes
#define YORU_MB(_n) ((_n) * YORU_KB(1000))   // 1 MB = 1000 KB
#define YORU_MiB(_n) ((_n) * YORU_KiB(1024)) // 1 MiB = 1024 KiB
#define YORU_GB(_n) ((_n) * YORU_MB(1000))   // 1 GB = 1000 MB
#define YORU_GiB(_n) ((_n) * YORU_MiB(1024)) // 1 GiB = 1024 MiB

/* ============================================================
   MODULE: ArrayList
   ============================================================ */

#define YORU_ARRAYLIST_INITIAL_CAPACITY (16)

#define Yoru_ArrayList_T(__T)                                                                                          \
  struct {                                                                                                             \
    __T            *items;                                                                                             \
    usize           size, capacity;                                                                                    \
    Yoru_Allocator *allocator;                                                                                         \
  }

#define yoru_arraylist_init(__arr_ptr, __allocator_ptr, __capacity)                                                    \
  do {                                                                                                                 \
    Yoru_Opt maybe_items =                                                                                             \
        yoru_allocator_alloc((__allocator_ptr), YORU_ARRAYLIST_INITIAL_CAPACITY * sizeof((__arr_ptr)->items[0]));      \
    assert(maybe_items.has_value && "could not allocate memory for arraylist");                                        \
    (__arr_ptr)->items     = maybe_items.ptr;                                                                          \
    (__arr_ptr)->size      = 0;                                                                                        \
    (__arr_ptr)->capacity  = ((__capacity) == 0) ? YORU_ARRAYLIST_INITIAL_CAPACITY : (__capacity);                     \
    (__arr_ptr)->allocator = __allocator_ptr;                                                                          \
  } while (0);

#define yoru_arraylist_append(__arr_ptr, __value)                                                                      \
  do {                                                                                                                 \
    assert((__arr_ptr));                                                                                               \
    if ((__arr_ptr)->size + 1 > (__arr_ptr)->capacity) {                                                               \
      yoru_arraylist_resize((__arr_ptr), (__arr_ptr)->capacity * 2);                                                   \
    }                                                                                                                  \
    (__arr_ptr)->items[(__arr_ptr)->size++] = __value;                                                                 \
  } while (0);

#define yoru_arraylist_destroy(__arr_ptr)                                                                              \
  do {                                                                                                                 \
    assert((__arr_ptr));                                                                                               \
    if (!(__arr_ptr)->items) {                                                                                         \
      yoru_allocator_dealloc((__arr_ptr)->allocator, (__arr_ptr)->items);                                              \
      (__arr_ptr)->items = NULL;                                                                                       \
    }                                                                                                                  \
    (__arr_ptr)->size      = 0;                                                                                        \
    (__arr_ptr)->capacity  = 0;                                                                                        \
    (__arr_ptr)->allocator = NULL;                                                                                     \
  } while (0)

#define yoru_arraylist_prepend(__arr_ptr, __value)                                                                     \
  do {                                                                                                                 \
    usize item_size = sizeof((__arr_ptr)->items[0]);                                                                   \
    assert((__arr_ptr));                                                                                               \
    if ((__arr_ptr)->size + 1 > (__arr_ptr)->capacity) {                                                               \
      yoru_arraylist_resize((__arr_ptr), (__arr_ptr)->capacity * 2);                                                   \
      \                                                                                                                \
    }                                                                                                                  \
    memmove((__arr_ptr)->items + 1, (__arr_ptr)->items, (__arr_ptr)->size * item_size);                                \
    (__arr_ptr)->items[0] = __value;                                                                                   \
    ++(__arr_ptr)->size;                                                                                               \
  } while (0);

#define yoru_arraylist_resize(__arr_ptr, __new_capacity)                                                               \
  do {                                                                                                                 \
    usize item_size = sizeof((__arr_ptr)->items[0]);                                                                   \
    assert((__arr_ptr));                                                                                               \
    Yoru_Opt maybe_new_ptr = yoru_allocator_realloc(                                                                   \
        (__arr_ptr)->allocator,                                                                                        \
        (__arr_ptr)->capacity * item_size,                                                                             \
        (__arr_ptr)->items,                                                                                            \
        (__new_capacity) * item_size);                                                                                 \
    assert(maybe_new_ptr.has_value && "could not append new value");                                                   \
    anyptr new_ptr        = maybe_new_ptr.ptr;                                                                         \
    (__arr_ptr)->items    = new_ptr;                                                                                   \
    (__arr_ptr)->size     = (__arr_ptr)->size < (__new_capacity) ? (__arr_ptr)->size : (__new_capacity);               \
    (__arr_ptr)->capacity = (__new_capacity);                                                                          \
  } while (0);

/* ============================================================
   MODULE: StringView
   ============================================================ */

/// @brief Immutable and sized NON-NULLTERMINATED c-string
typedef struct Yoru_String {
  u8             *data;
  usize           length;
  Yoru_Allocator *allocator;
} Yoru_String;

bool yoru_string_make(Yoru_Allocator *allocator, usize length, Yoru_String *out_string);
bool yoru_string_from_str(Yoru_Allocator *allocator, const char *s, usize length, Yoru_String *out_string);
bool yoru_string_copy(Yoru_Allocator *allocator, Yoru_String *src, Yoru_String *dest);
bool yoru_string_substring(Yoru_String *s, usize start, usize end, Yoru_String *out_string);
void yoru_string_destroy(Yoru_String *s);

#ifdef YORU_IMPL
bool yoru_string_make(Yoru_Allocator *allocator, usize length, Yoru_String *out_string) {
  assert(allocator);
  assert(out_string);

  Yoru_Opt maybe_data = yoru_allocator_alloc(allocator, length * sizeof(u8));
  if (!maybe_data.has_value) return false;

  out_string->data      = maybe_data.ptr;
  out_string->length    = length;
  out_string->allocator = allocator;
  return true;
}

bool yoru_string_from_str(Yoru_Allocator *allocator, const char *s, usize length, Yoru_String *out_string) {
  if (!yoru_string_make(allocator, length, out_string)) return false;
  for (usize i = 0; i < length; ++i)
    out_string->data[i] = (u8)s[i];

  return true;
}

bool yoru_string_copy(Yoru_Allocator *allocator, Yoru_String *src, Yoru_String *dest) {
  assert(allocator);
  assert(src);
  assert(dest);

  if (!yoru_string_make(allocator, src->length, dest)) return false;
  memcpy(dest->data, src->data, src->length);
  return true;
}

bool yoru_string_substring(Yoru_String *s, usize start, usize end, Yoru_String *out_string) {
  assert(out_string);
  assert(s);
  assert(s->data);
  assert(s->allocator);
  assert(start < s->length && "index must be within length");
  assert(end < s->length && "index must be within length");
  assert(start < end && "start index must be less than end");

  usize length          = end - start;
  out_string->data      = s->data + start;
  out_string->length    = length;
  out_string->allocator = s->allocator;
  return true;
}

void yoru_string_destroy(Yoru_String *s) {
  assert(s);
  assert(s->data);
  assert(s->allocator);

  yoru_allocator_dealloc(s->allocator, s->data);
  s->data      = NULL;
  s->length    = 0;
  s->allocator = NULL;
}
#endif // YORU_IMPL

/* ============================================================
   MODULE: StringBuilder
   ============================================================ */

typedef Yoru_ArrayList_T(u8) Yoru_StringBuilder;

void yoru_stringbuilder_init(Yoru_Allocator *allocator, Yoru_StringBuilder *sb);
void yoru_stringbuilder_destroy(Yoru_StringBuilder *sb);
bool yoru_stringbuilder_append_char(Yoru_StringBuilder *sb, char c);
bool yoru_stringbuilder_append_cstr(Yoru_StringBuilder *sb, const char *, usize length);
bool yoru_stringbuilder_append_string(Yoru_StringBuilder *sb, const Yoru_String *s);
bool yoru_stringbuilder_to_string(Yoru_StringBuilder *sb, Yoru_String *out_string);

#ifdef YORU_IMPL
void yoru_stringbuilder_init(Yoru_Allocator *allocator, Yoru_StringBuilder *sb) {
  assert(sb);
  assert(allocator);
  yoru_arraylist_init(sb, allocator, 0); // uses default initial capacity
}

void yoru_stringbuilder_destroy(Yoru_StringBuilder *sb) {
  assert(sb);
  yoru_arraylist_destroy(sb);
}

bool yoru_stringbuilder_to_string(Yoru_StringBuilder *sb, Yoru_String *out_string) {
  assert(sb);
  assert(sb->items);
  assert(sb->allocator);
  assert(out_string);
  Yoru_String temp_res = {0};
  if (!yoru_string_from_str(sb->allocator, (const char *)sb->items, sb->size, &temp_res)) return false;
  if (!yoru_string_copy(sb->allocator, &temp_res, out_string)) return false;
  return true;
}

bool yoru_stringbuilder_append_char(Yoru_StringBuilder *sb, char c) {
  assert(sb);
  assert(sb->items);
  assert(sb->allocator);
  yoru_arraylist_append(sb, (u8)c);
  return true;
}

bool yoru_stringbuilder_append_cstr(Yoru_StringBuilder *sb, const char *cstr, usize length) {
  assert(sb);
  assert(sb->items);
  assert(sb->allocator);
  assert(cstr);

  for (usize i = 0; i < length; ++i)
    if (!yoru_stringbuilder_append_char(sb, (u8)cstr[i])) return false;

  return true;
}

bool yoru_stringbuilder_append_string(Yoru_StringBuilder *sb, const Yoru_String *s) {
  assert(sb);
  assert(sb->items);
  assert(sb->allocator);
  assert(s);

  for (usize i = 0; i < s->length; ++i)
    if (!yoru_stringbuilder_append_char(sb, s->data[i])) return false;

  return true;
}
#endif // YORU_IMPL

/* ============================================================
   MODULE: FileSystem
   ============================================================ */

Yoru_String yoru_read_file(Yoru_Allocator *allocator, const char *filepath);

#ifdef YORU_IMPL
Yoru_String yoru_read_file(Yoru_Allocator *allocator, const char *filepath) {
  assert(allocator);
  assert(filepath);

  Yoru_String res = {0};

  FILE *file = fopen(filepath, "r");
  if (!file) goto cleanup;

  fseek(file, 0, SEEK_END);
  usize size = ftell(file);

  if (!yoru_string_make(allocator, size, &res)) goto cleanup;
  fread(res.data, sizeof(u8), res.length, file);
  fclose(file);
  return res;

cleanup:
  if (file) fclose(file);
  return res;
}
#endif // YORU_IMPL

#endif // __YORU_H__
