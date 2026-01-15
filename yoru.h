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
#include <ctype.h>

#if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
#  include <sys/mman.h>
#  include <unistd.h>
#endif

#if defined(_WIN32)
#  include <windows.h>
#endif

/* ============================================================
   MODULE: Types
   provides common typedefs for fixed size types
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

#define YORU_ARRAY_LEN(__arr) (sizeof(arr) / sizeof(arr[0]))
#define YORU_TODO(__msg) assert(false && __msg)
#define YORU_NAMEOF(__x) (#__x)

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
   provides the concept of an optional value
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

/// @brief Represents a ptr that MUST NOT be null. (unless youre silly! ^^)
/// Please make sure to use the provided constructor `yoru_ref_make` to make a new ref
/// and use `yoru_ref_get` to get the actual ptr contained in the ref struct.
/// If in any way the ptr is null when creating the ref or getting the ptr it will fail
/// because the `contract` is broken.
typedef struct Yoru_Ref {
  const anyptr ptr;
} Yoru_Ref;

/// @brief Creates a reference
Yoru_Ref yoru_ref_make(const anyptr ptr);

/// @brief Gets the pointer to the non null reference. However, if it were
/// to be null then the program would crash.
anyptr yoru_ref_get(const Yoru_Ref *ref);

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

/// @brief Functions that an allocator must implement.
/// If a function would be a no-op, they would still need to be provided.
typedef struct Yoru_AllocatorVTable {
  Yoru_Allocator_Alloc_Func   alloc;
  Yoru_Allocator_DeAlloc_Func dealloc;
  Yoru_Allocator_ReAlloc_Func realloc;
  Yoru_Allocator_Destroy_Func destroy;
} Yoru_AllocatorVTable;

/// @brief Allocator Interface
typedef struct Yoru_Allocator {
  const Yoru_AllocatorVTable *vtable;
  anyptr                      ctx;
} Yoru_Allocator;

/// @brief Allocates memory using the alloc function inside the allocators vtable
Yoru_Opt yoru_allocator_alloc(Yoru_Allocator *allocator, usize size);

/// @brief De-Allocates/Frees memory using the dealloc function inside the allocators vtable
void yoru_allocator_dealloc(Yoru_Allocator *allocator, anyptr ptr);

/// @brief Re-Allocates memory using the realloc function inside the allocators vtable
Yoru_Opt yoru_allocator_realloc(Yoru_Allocator *allocator, usize old_size, anyptr old_ptr, usize new_size);

// @brief Destroys the allocator instance using the destroy function iside the allocators vtable
void yoru_allocator_destroy(Yoru_Allocator *allocator);

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
   provides a global allocator that is just a wrapper around
   calloc and free that fits the ALlocator interface
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
   provides an arena allocator that allocates the entire
   capacity once when creating the allocator.

   If you do not wish to allocate everything at once, please
   take a look at the `VirtualArenaAllocator`
   ============================================================ */

typedef Yoru_Allocator Yoru_ArenaAllocator;

/// @brief Creates an arena allocator
Yoru_ArenaAllocator *yoru_arena_allocator_make(usize capacity);

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
  if (!a) goto err;
  a->vtable = &__yoru_arena_allocator_vtable;

  ctx = calloc(1, sizeof(Yoru_ArenaAllocatorCtx));
  if (!ctx) goto err;
  a->ctx = (anyptr)ctx;

  mem = (byte *)calloc(1, capacity);
  if (!mem) goto err;
  ctx->mem      = mem;
  ctx->offset   = 0;
  ctx->capacity = capacity;
  return a;

err:
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

     -> TODO: maybe we could handle reallocs like normal allocations instead and just push to the arena?
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

#if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__)) || defined(_WIN32) || defined(_WIN32)
/* ============================================================
   MODULE: VirtualMemory
   provides an interface to work with virtual memory on platforms such as
     - linux
     - macos
     - windows
     -> if your platform is not supported you can open an issue or open a pull request! :)
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

#  ifdef YORU_IMPL
usize yoru_align_up(usize x, usize alignment) {
  return (x + alignment - 1) & ~(alignment - 1);
}

usize yoru_get_page_size() {
#    if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
  return (usize)sysconf(_SC_PAGE_SIZE);
#    elif defined(_WIN32)
  SYSTEM_INFO sysinfo = {0};
  GetSystemInfo(&sysinfo);
  return (usize)sysinfo.dwPageSize;
#    else
#      error "platform not supported yet"
#    endif
}

static inline bool __yoru_vmem_reserve_linux(usize size, Yoru_Vmem_Ctx *ctx);
static inline bool __yoru_vmem_reserve_windows(usize size, Yoru_Vmem_Ctx *ctx);

bool yoru_vmem_reserve(usize size, Yoru_Vmem_Ctx *out_ctx) {
  assert(out_ctx && "must not be null");
#    if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
  return __yoru_vmem_reserve_linux(size, out_ctx);
#    elif defined(_WIN32)
  return __yoru_vmem_reserve_windows(size, out_ctx);
#    else
#      error "platform not supported yet"
#    endif
}

bool __yoru_vmem_commit_linux(Yoru_Vmem_Ctx *ctx, usize size);
bool __yoru_vmem_commit_windows(Yoru_Vmem_Ctx *ctx, usize size);

bool yoru_vmem_commit(Yoru_Vmem_Ctx *ctx, usize size) {
  assert(ctx && "must not be null");
  assert(ctx->base && "must not be null");
#    if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
  return __yoru_vmem_commit_linux(ctx, size);
#    elif defined(_WIN32)
  return __yoru_vmem_commit_windows(ctx, size);
#    else
#      error "platform not supported yet"
#    endif
}

static inline bool __yoru_vmem_free_linux(Yoru_Vmem_Ctx *ctx);
static inline bool __yoru_vmem_free_windows(Yoru_Vmem_Ctx *ctx);

bool yoru_vmem_free(Yoru_Vmem_Ctx *ctx) {
  assert(ctx && "must not be null");
  assert(ctx->base && "must not be null");
#    if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
  return __yoru_vmem_free_linux(ctx);
#    elif defined(_WIN32)
  return __yoru_vmem_free_windows(ctx);
#    else
#      error "platform not supported yet"
#    endif
}

#    if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
static inline bool __yoru_vmem_reserve_linux(usize size, Yoru_Vmem_Ctx *ctx) {
  anyptr ptr = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
  if (ptr == MAP_FAILED) {
    ctx->base            = NULL;
    ctx->commit_pos      = 0;
    ctx->addr_space_size = 0;
    return false;
  }

  ctx->base            = ptr;
  ctx->commit_pos      = 0;
  ctx->addr_space_size = size;
  return true;
}

bool __yoru_vmem_commit_linux(Yoru_Vmem_Ctx *ctx, usize size) {
  usize size_aligned = yoru_align_up(size, yoru_get_page_size());
  int   err          = mprotect((u8 *)ctx->base + ctx->commit_pos, size_aligned, PROT_READ | PROT_WRITE);
  if (err != 0) return false;
  ctx->commit_pos += size_aligned;
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
#    endif

#    if defined(_WIN32)
static inline bool __yoru_vmem_reserve_windows(usize size, Yoru_Vmem_Ctx *ctx) {
  anyptr ptr = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);
  if (!ptr) {
    ctx->base            = NULL;
    ctx->commit_pos      = 0;
    ctx->addr_space_size = 0;
    return false;
  }

  ctx->base            = ptr;
  ctx->commit_pos      = 0;
  ctx->addr_space_size = size;
  return true;
}

bool __yoru_vmem_commit_windows(Yoru_Vmem_Ctx *ctx, usize size) {
  usize  size_aligned = yoru_align_up(size, yoru_get_page_size());
  anyptr ptr          = VirtualAlloc((u8 *)ctx->base + ctx->commit_pos, size_aligned, MEM_COMMIT, PAGE_READWRITE);
  if (!ptr) return false;
  ctx->commit_pos += size_aligned;
  return true;
}

static inline bool __yoru_vmem_free_windows(Yoru_Vmem_Ctx *ctx) {
  if (!VirtualFree(ctx->base, ctx->addr_space_size, MEM_RELEASE)) return false;
  ctx->base            = NULL;
  ctx->commit_pos      = 0;
  ctx->addr_space_size = 0;
  return true;
}
#    endif // Platform Check

#  endif // YORU_IMPL
#else
#  error "platform not supported yet"
#endif // Platform Check

#if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__)) || defined(_WIN32)
/* ============================================================
   MODULE: VirtualArenaAllocator
   provides an arena allocator that commits pages in a reserved
   address space instead of allocating the entire capacity when
   creating the allocator (as is the case with `ArenaAllocator`)

   Makes use of the VirtualMemory module and therefore is only
   supported by platforms where the VirtualMemory module
   is defined.
   ============================================================ */

typedef Yoru_Allocator      Yoru_VirtualArenaAllocator;
Yoru_VirtualArenaAllocator *yoru_virtual_arena_allocator_make(usize capacity);

#  ifdef YORU_IMPL
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

Yoru_VirtualArenaAllocator *yoru_virtual_arena_allocator_make(usize capacity) {
  Yoru_VirtualArenaAllocator *a = calloc(1, sizeof(Yoru_VirtualArenaAllocator));
  if (!a) return NULL;

  Yoru_VirtualArenaAllocatorCtx *ctx = calloc(1, sizeof(Yoru_VirtualArenaAllocatorCtx));
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
  c->offset   = 0;
  c->vmem_ctx = NULL;
}
#  endif // YORU_IMPL
#endif   // Platform Check

#define YORU_B(_n) ((usize)1 * (_n))
#define YORU_KB(_n) ((_n) * YORU_B(1000))    // 1 KB = 1000 bytes
#define YORU_KiB(_n) ((_n) * YORU_B(1024))   // 1 KiB = 1024 bytes
#define YORU_MB(_n) ((_n) * YORU_KB(1000))   // 1 MB = 1000 KB
#define YORU_MiB(_n) ((_n) * YORU_KiB(1024)) // 1 MiB = 1024 KiB
#define YORU_GB(_n) ((_n) * YORU_MB(1000))   // 1 GB = 1000 MB
#define YORU_GiB(_n) ((_n) * YORU_MiB(1024)) // 1 GiB = 1024 MiB

/* ============================================================
   MODULE: ArrayList
   provides an interface to create dynamic arrays by for example
   creating a new typedef for your ArrayList type or add
   it as a field in a struct:
   ```c
   typedef Yoru_ArrayList_T(int) IntArrayList;

   // ... oooor you could also just add it as a field inside a struct,
   // typedef struct {
   //   Yoru_ArrayList_T(int) values;
   // } OtherIntArrayList;

   void my_func() {
     Yoru_Allocator allocator = yoru_global_allocator_make();
     IntArrayList xs = {0};

     // you can just pass 0 as initial capacity and it will use the default
     yoru_arraylist_init(&xs, &allocator, 16); 
     for (usize i = 0; i < 128; ++i) {
       yoru_arraylist_append(&xs, i);
     }

     // do something

     yoru_arraylist_destroy(&xs); // destroys arraylist with allocator that `owns` it
   }
   ```
   
   TODO:
     - find a way to handle errors better
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
   provides an immutable, sized, non-nulltermianted stringview type
   that does NOT own its data and is just a view into an already
   existing string.
   ============================================================ */

#define Yoru_String_Fmt "%.*s"
#define Yoru_String_Fmt_Args(__str_ptr) (int)(__str_ptr)->length, (__str_ptr)->data

/// @brief immutable and sized NON-NULLTERMINATED string
typedef struct {
  const u8 *data;
  usize     length;
} Yoru_StringView;

typedef Yoru_ArrayList_T(Yoru_StringView) Yoru_StringViews;
typedef bool (*Yoru_CharPredicate)(u8 c, usize index);

typedef enum { YORU_TRIM_LEFT = 1, YORU_TRIM_RIGHT = 2 } Yoru_TrimOptions;

Yoru_StringView yoru_stringview_skip(const Yoru_StringView *sv, usize skip);
Yoru_StringView yoru_stringview_skip_while(const Yoru_StringView *sv, Yoru_CharPredicate predicate);
Yoru_StringView yoru_stringview_take(const Yoru_StringView *sv, usize take);
Yoru_StringView yoru_stringview_take_while(const Yoru_StringView *sv, Yoru_CharPredicate predicate);
bool            yoru_stringview_has_prefix(const Yoru_StringView *sv, const char *prefix, usize n);
bool            yoru_stringview_has_infix(const Yoru_StringView *sv, const char *infix, usize n);
bool            yoru_stringview_has_suffix(const Yoru_StringView *sv, const char *suffix, usize n);
bool            yoru_stringview_is_empty(const Yoru_StringView *sv);
Yoru_StringView yoru_stringview_trim(const Yoru_StringView *sv, Yoru_TrimOptions trim_options);

Yoru_StringView
yoru_stringview_trim_while(const Yoru_StringView *sv, Yoru_TrimOptions trim_options, Yoru_CharPredicate predicate);

Yoru_StringViews yoru_stringview_split_by_char(
    const Yoru_StringView *sv, Yoru_Allocator *allocator, usize max_split_count, char separator, bool remove_empty);

#ifdef YORU_IMPL
Yoru_StringView yoru_stringview_skip(const Yoru_StringView *sv, usize skip) {
  assert(sv);
  skip              = skip < sv->length ? skip : sv->length;
  Yoru_StringView s = *sv;
  return (Yoru_StringView){.data = s.data + skip, .length = s.length - skip};
}

Yoru_StringView yoru_stringview_skip_while(const Yoru_StringView *sv, Yoru_CharPredicate predicate) {
  assert(sv);
  assert(predicate);
  Yoru_StringView s = *sv;
  usize           i = 0;
  while (i < s.length) {
    char c = s.data[i];
    if (!predicate(c, i)) break;
    ++i;
  }

  return (Yoru_StringView){.data = s.data + i, .length = s.length - i};
}

Yoru_StringView yoru_stringview_take(const Yoru_StringView *sv, usize take) {
  assert(sv);
  Yoru_StringView s = *sv;
  take              = take < s.length ? take : s.length;
  return (Yoru_StringView){.data = s.data, .length = take};
}

Yoru_StringView yoru_stringview_take_while(const Yoru_StringView *sv, Yoru_CharPredicate predicate) {
  assert(sv);
  assert(predicate);
  Yoru_StringView s = *sv;

  usize i = 0;
  while (i < s.length) {
    char c = s.data[i];
    if (!predicate(c, i)) break;
    ++i;
  }

  return (Yoru_StringView){.data = s.data, .length = i};
}

bool yoru_stringview_has_prefix(const Yoru_StringView *sv, const char *prefix, usize n) {
  assert(sv);
  assert(sv->data);
  if (sv->length < n) return false;
  return memcmp(sv->data, prefix, n) == 0;
}

bool yoru_stringview_has_infix(const Yoru_StringView *sv, const char *infix, usize n) {
  assert(sv);
  assert(sv->data);
  if (sv->length < n) return false;
  for (usize i = 0; i < sv->length; ++i) {
    if (sv->data[i] == (u8)infix[0] && (i + n < sv->length)) {
      usize pos = i + 1;
      while (pos < sv->length) {
        if (!(sv->data[pos] == (u8)infix[pos])) break;
        ++pos;
      }

      // if we get here then we found a match because we dont break out of the loop
      // because of a diff of infix and data
      return true;
    }
  }

  return false;
}

bool yoru_stringview_has_suffix(const Yoru_StringView *sv, const char *suffix, usize n) {
  assert(sv);
  assert(sv->data);
  if (sv->length < n) return false;
  return memcmp(sv->data + sv->length - n, suffix, n) == 0;
}

bool yoru_stringview_is_empty(const Yoru_StringView *sv) {
  assert(sv);
  return sv->length == 0;
}

Yoru_StringView yoru_stringview_trim(const Yoru_StringView *sv, Yoru_TrimOptions trim_options) {
  assert(sv);

  usize start = 0;
  usize end   = sv->length; // exclusive

  if (trim_options & YORU_TRIM_LEFT) {
    while (start < end && isspace((unsigned char)sv->data[start])) {
      ++start;
    }
  }

  if (trim_options & YORU_TRIM_RIGHT) {
    while (end > start && isspace((unsigned char)sv->data[end - 1])) {
      --end;
    }
  }

  return (Yoru_StringView){.data = sv->data + start, .length = end - start};
}

Yoru_StringView
yoru_stringview_trim_while(const Yoru_StringView *sv, Yoru_TrimOptions trim_options, Yoru_CharPredicate predicate) {
  assert(sv);
  assert(predicate);

  usize start = 0;
  usize end   = sv->length; // exclusive

  if (trim_options & YORU_TRIM_LEFT) {
    while (start < end && predicate(sv->data[start], start)) {
      ++start;
    }
  }

  if (trim_options & YORU_TRIM_RIGHT) {
    while (end > start && predicate(sv->data[end - 1], end - 1)) {
      --end;
    }
  }

  return (Yoru_StringView){
      .data   = sv->data + start,
      .length = end - start,
  };
}

Yoru_StringViews yoru_stringview_split_by_char(
    const Yoru_StringView *sv, Yoru_Allocator *allocator, usize max_split_count, char separator, bool remove_empty) {
  assert(sv && sv->data && allocator);

  max_split_count = max_split_count == USIZE_MAX ? YORU_ARRAYLIST_INITIAL_CAPACITY : max_split_count;

  Yoru_StringViews stringviews = {0};
  yoru_arraylist_init(&stringviews, allocator, max_split_count);

  usize curr_start = 0;

  for (usize i = 0; i < sv->length; ++i) {
    if (sv->data[i] != separator) continue;

    if (i == curr_start && remove_empty) {
      curr_start = i + 1;
      continue;
    }

    Yoru_StringView field = {.data = sv->data + curr_start, .length = i - curr_start};

    yoru_arraylist_append(&stringviews, field);
    curr_start = i + 1;
  }

  // append last field
  if (curr_start < sv->length) {
    usize length = sv->length - curr_start;
    if (!(remove_empty && length == 0)) {
      Yoru_StringView field = {.data = sv->data + curr_start, .length = length};
      yoru_arraylist_append(&stringviews, field);
    }
  }

  return stringviews;
}

#endif // YORU_IMPL

/* ============================================================
   MODULE: String
   provides an immutable, sized, non-nulltermianted string type
   that OWNS its data.

   Difference to StringView:
   - StringView does not own the memory it is 'viewing'. StringViews
     can be created from a StringBuilder or a String itself but
     they do not allocate nor do they free memory.
  ============================================================= */

typedef struct {
  const u8       *data;
  usize           length;
  Yoru_Allocator *allocator;
} Yoru_String;

/// @brief Creates an empty string of specific length. If `initial_value` is NULL then the string is just a zero string.
bool yoru_string_make(Yoru_Allocator *allocator, usize length, const char *initial_value, Yoru_String *out_string);

/// @brief Creates a DEEP-copy of a string
bool yoru_string_copy(Yoru_Allocator *allocator, Yoru_String *src, Yoru_String *dest);

/// @brief Deallocates the string using the allocator that owns it
void yoru_string_destroy(Yoru_String *s);

/// @brief Creates a new string which is the substring of a given string
/// @note Changing values in the substring also changes values in the "complete" string as it is just a small window/view of the original string
bool yoru_string_substring(Yoru_String *s, usize start, usize end, Yoru_StringView *out_stringview);

#ifdef YORU_IMPL
bool yoru_string_make(Yoru_Allocator *allocator, usize length, const char *initial_value, Yoru_String *out_string) {
  assert(allocator);
  assert(out_string);

  Yoru_Opt maybe_data = yoru_allocator_alloc(allocator, length * sizeof(u8));
  if (!maybe_data.has_value) return false;
  if (initial_value != NULL) { memcpy(maybe_data.ptr, initial_value, length); }

  out_string->data      = maybe_data.ptr;
  out_string->length    = length;
  out_string->allocator = allocator;
  return true;
}

bool yoru_string_copy(Yoru_Allocator *allocator, Yoru_String *src, Yoru_String *dest) {
  assert(allocator);
  assert(src);
  assert(dest);

  if (!yoru_string_make(allocator, src->length, NULL, dest)) return false;
  memcpy((anyptr)dest->data, src->data, src->length);
  return true;
}

bool yoru_string_substring(Yoru_String *s, usize start, usize end, Yoru_StringView *out_stringview) {
  assert(out_stringview);
  assert(s);
  assert(s->data);
  assert(s->allocator);
  assert(start < s->length && "index must be within length");
  assert(end < s->length && "index must be within length");
  assert(start < end && "start index must be less than end");

  usize length           = end - start;
  out_stringview->data   = s->data + start;
  out_stringview->length = length;
  return true;
}

void yoru_string_destroy(Yoru_String *s) {
  assert(s);
  assert(s->data);
  assert(s->allocator);
  yoru_allocator_dealloc(s->allocator, (anyptr)s->data);
  s->data      = NULL;
  s->length    = 0;
  s->allocator = NULL;
}
#endif // YORU_IMPL

/* ============================================================
   MODULE: StringBuilder
   provides a way to 'build' strings into a dynamic array
   by appending char, c-strings or Yoru_String.

   TOOD:
     - add a `yoru_stringbuilder_append_format(format, ...)` instead of char and cstr func
   ============================================================ */

typedef Yoru_ArrayList_T(u8) Yoru_StringBuilder;

/// @brief Initializese the stringbuilder (creating the dynamic array)
void yoru_stringbuilder_init(Yoru_Allocator *allocator, Yoru_StringBuilder *sb);

/// @brief Destroys / Deallocates the string builder
void yoru_stringbuilder_destroy(Yoru_StringBuilder *sb);

/// @brief Appends a char to the stringbuilder
bool yoru_stringbuilder_append_char(Yoru_StringBuilder *sb, char c);

/// @brief Appends a c-string of a given length to the stringbuilder
bool yoru_stringbuilder_append_cstr(Yoru_StringBuilder *sb, const char *, usize length);

/// @brief Appends a Yoru_String to the stringbuilder
bool yoru_stringbuilder_append_string(Yoru_StringBuilder *sb, const Yoru_String *s);

/// @brief Creates a string from the stringbuider, copying the data of the stringbuilder
bool yoru_stringbuilder_to_string(Yoru_StringBuilder *sb, Yoru_String *out_string);

/// @brief Creates a stringview from the current stringbuilder state
bool yoru_stringbuilder_to_stringview(Yoru_StringBuilder *sb, Yoru_StringView *out_sv);

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
  if (!yoru_string_make(sb->allocator, sb->size, (const char *)sb->items, out_string)) return false;
  return true;
}

bool yoru_stringbuilder_to_stringview(Yoru_StringBuilder *sb, Yoru_StringView *out_sv) {
  assert(sb);
  assert(sb->items);
  assert(sb->allocator);
  assert(out_sv);
  out_sv->data   = sb->items;
  out_sv->length = sb->size;
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
   provides a small interface to interact with filesystem
   ============================================================ */

Yoru_String yoru_file_read(Yoru_Allocator *allocator, const char *filepath);
Yoru_String yoru_file_read_exact(Yoru_Allocator *allocator, const char *filepath, usize offset_bytes, usize max_bytes);
usize       yoru_file_get_size(const char *filepath);

#ifdef YORU_IMPL
Yoru_String yoru_file_read(Yoru_Allocator *allocator, const char *filepath) {
  assert(allocator);
  assert(filepath);
  usize size = yoru_file_get_size(filepath);
  return yoru_file_read_exact(allocator, filepath, 0, size);
}

Yoru_String yoru_file_read_exact(Yoru_Allocator *allocator, const char *filepath, usize offset_bytes, usize max_bytes) {
  assert(allocator);
  assert(filepath);

  Yoru_String res  = {0};
  FILE       *file = fopen(filepath, "rb");
  if (!file) goto cleanup;

  fseek(file, 0, SEEK_END);
  usize end_pos   = ftell(file);
  usize file_size = end_pos - offset_bytes;
  if (offset_bytes >= file_size) goto cleanup;

  // make sure that we do not try to read more than we can
  usize read_size = file_size - offset_bytes;
  if (read_size > max_bytes) read_size = max_bytes;
  if (!yoru_string_make(allocator, read_size, NULL, &res)) goto cleanup;

  fseek(file, offset_bytes, SEEK_SET);
  fread((anyptr)res.data, sizeof(u8), res.length, file);
  fclose(file);
  return res;

cleanup:
  if (file) fclose(file);
  res.data = NULL;
  return res;
}

usize yoru_file_get_size(const char *filepath) {
  assert(filepath);
  FILE *file = fopen(filepath, "r");
  if (!file) return 0;
  fseek(file, 0, SEEK_END);
  usize size = ftell(file);
  fclose(file);
  return size;
}
#endif // YORU_IMPL

#endif // __YORU_H__
