#ifndef __YORU_H__
#define __YORU_H__

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#define YORU_ARRAY_LEN(__arr) (sizeof(__arr) / sizeof(__arr[0]))
#define YORU_TODO(__msg) assert(false && "TODO: "__msg)
#define YORU_NAMEOF(__x) (#__x)
#define YORU_MAX(_a, _b) ((_a) < (_b) ? (_b) : (_a))
#define YORU_MIN(_a, _b) ((_a) < (_b) ? (_a) : (_b))

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
/// Please make sure to use the provided constructor `yoru_ref_make` to make a
/// new ref and use `yoru_ref_get` to get the actual ptr contained in the ref
/// struct. If in any way the ptr is null when creating the ref or getting the
/// ptr it will fail because the `contract` is broken.
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

/// @brief Allocates memory using the alloc function inside the allocators
/// vtable
Yoru_Opt yoru_allocator_alloc(Yoru_Allocator *allocator, usize size);

/// @brief De-Allocates/Frees memory using the dealloc function inside the
/// allocators vtable
void yoru_allocator_dealloc(Yoru_Allocator *allocator, anyptr ptr);

/// @brief Re-Allocates memory using the realloc function inside the allocators
/// vtable
Yoru_Opt yoru_allocator_realloc(Yoru_Allocator *allocator, usize old_size, anyptr old_ptr, usize new_size);

// @brief Destroys the allocator instance using the destroy function iside the
// allocators vtable
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

   This is for individual allocations and frees unlike the
   `ArenaAllocator` or the `VirtualArenaAllocator`.
   ============================================================ */

typedef Yoru_Allocator Yoru_GlobalAllocator;

/// @brief Creates a global allocator
Yoru_GlobalAllocator yoru_global_allocator_make();

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

/// @brief Creates a heap-based arena allocator
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
  /* Freeing in an arena is a no-op because the idea is that you free the entire
     arena once. For that you would call the yoru_allocator_destroy func on the
     arena allocator */
}

Yoru_Opt __yoru_arena_allocator_realloc(anyptr ctx, usize old_size, anyptr old_ptr, usize new_size) {
  (void)ctx;
  (void)old_size;
  (void)old_ptr;
  (void)new_size;
  /* Reallocations are not supported on arena allocators because if you want to
     grow something in the middle of the arena then we would also have to shift
     every ptr to the right which might not work becuase the arena might not be
     able to fit everything with the new ptr in the arena and we dont know in
     the arena ctx where which ptr starts! otherwise we would have to keep track
     of them here.
     -> TODO: maybe we could handle reallocs like normal allocations instead and
     just push to the arena?
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
     -> if your platform is not supported you can open an issue or open a pull
   request! :)
   ============================================================ */

typedef struct Yoru_Vmem_Ctx {
  anyptr base;
  usize  commit_pos;
  usize  addr_space_size;
} Yoru_Vmem_Ctx;

/// @brief aligns `x` to `alignment`
usize yoru_align_up(usize x, usize alignment);

/// @brief returns the page size on the current system
usize yoru_get_page_size();

/// @brief reserves an page-aligned `size` amount of bytes of reserved memory
bool yoru_vmem_reserve(usize size, Yoru_Vmem_Ctx *out_ctx);

/// @brief commits a page-aligned size to a `ctx` increasing the commit
/// position. returns true on success, else false
bool yoru_vmem_commit(Yoru_Vmem_Ctx *ctx, usize size);

/// @brief frees the reserved address space
bool yoru_vmem_free(Yoru_Vmem_Ctx *ctx);

#  ifdef YORU_IMPL
usize yoru_align_up(usize x, usize alignment) {
  return (x + alignment - 1) & ~(alignment - 1);
}

#    if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
bool __yoru_vmem_reserve_linux(usize size, Yoru_Vmem_Ctx *ctx);
bool __yoru_vmem_commit_linux(Yoru_Vmem_Ctx *ctx, usize size);
bool __yoru_vmem_free_linux(Yoru_Vmem_Ctx *ctx);
#    elif defined(_WIN32)
bool __yoru_vmem_reserve_windows(usize size, Yoru_Vmem_Ctx *ctx);
bool __yoru_vmem_commit_windows(Yoru_Vmem_Ctx *ctx, usize size);
bool __yoru_vmem_free_windows(Yoru_Vmem_Ctx *ctx);
#    else
#      error "platform not supported"
#    endif

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
bool __yoru_vmem_reserve_linux(usize size, Yoru_Vmem_Ctx *ctx) {
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

bool __yoru_vmem_free_linux(Yoru_Vmem_Ctx *ctx) {
  int err = munmap(ctx->base, ctx->addr_space_size);
  if (err != 0) return false;
  ctx->base            = NULL;
  ctx->commit_pos      = 0;
  ctx->addr_space_size = 0;
  return true;
}
#    endif
#    if defined(_WIN32)
bool __yoru_vmem_reserve_windows(usize size, Yoru_Vmem_Ctx *ctx) {
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

bool __yoru_vmem_free_windows(Yoru_Vmem_Ctx *ctx) {
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

typedef Yoru_Allocator Yoru_VirtualArenaAllocator;

/// @brief Creates an instance of a `VirtualArenaAllocator` with one committed
/// page.
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
  /* Freeing in an arena is a no-op because the idea is that you free the entire
     arena once. For that you would call the yoru_allocator_destroy func on the
     arena allocator */
}

Yoru_Opt __yoru_virtual_arena_allocator_realloc(anyptr ctx, usize old_size, anyptr old_ptr, usize new_size) {
  (void)ctx;
  (void)old_size;
  (void)old_ptr;
  (void)new_size;
  /* Reallocations are not supported on arena allocators because if you want to
     grow something in the middle of the arena then we would also have to shift
     every ptr to the right which might not work becuase the arena might not be
     able to fit everything with the new ptr in the arena and we dont know in
     the arena ctx where which ptr starts! otherwise we would have to keep track
     of them here.

     -> maybe we could handle reallocs like normal allocations instead and just
     push to the arena?
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

     yoru_arraylist_destroy(&xs); // destroys arraylist with allocator that
   `owns` it
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
    (__arr_ptr)->items[(__arr_ptr)->size++] = (__value);                                                               \
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

#define yoru_arraylist_fill(__arr_ptr, __value)                                                                        \
  do {                                                                                                                 \
    assert((__arr_ptr));                                                                                               \
    assert((__arr_ptr)->items);                                                                                        \
    for (usize __i = 0; __i < (__arr_ptr)->size; ++__i) {                                                              \
      (__arr_ptr)->items[__i] = __value;                                                                               \
    }                                                                                                                  \
    (__arr_ptr)->size = (__arr_ptr)->capacity;                                                                         \
  } while (0);

#define yoru_arraylist_clear(__arr_ptr)                                                                                \
  do {                                                                                                                 \
    assert((__arr_ptr));                                                                                               \
    assert((__arr_ptr)->items);                                                                                        \
    memset((__arr_ptr)->items, 0, (__arr_ptr)->capacity * sizeof((__arr_ptr)->items[0]));                              \
    (__arr_ptr)->size = 0;                                                                                             \
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
   MODULE: HashMap
   provides a typesafe hashmap...

   TODO: better error handling... same as with arraylists
   ============================================================ */

usize yoru_hash_djb2(const char *str);

#define YORU_HASHMAP_INITIAL_CAPACITY (16)

#define YORU_HASHMAP_LOAD_FACTOR (0.75)

#define Yoru_HashMap_Entry_T(__T)                                                                                      \
  struct {                                                                                                             \
    cstr key;                                                                                                          \
    __T  value;                                                                                                        \
    bool set;                                                                                                          \
  }

typedef struct {
  cstr  key;
  usize index;
} Yoru_IndexedKey;

#define Yoru_HashMap_Entries_T(__T) Yoru_ArrayList_T(Yoru_HashMap_Entry_T(__T))

#define Yoru_HashMap_T(__T)                                                                                            \
  struct {                                                                                                             \
    Yoru_HashMap_Entries_T(__T) entries;                                                                               \
    Yoru_ArrayList_T(Yoru_IndexedKey) keys;                                                                            \
    Yoru_Allocator *allocator;                                                                                         \
  }

#define yoru_hashmap_init(__map_ptr, __allocator_ptr)                                                                  \
  do {                                                                                                                 \
    assert((__map_ptr));                                                                                               \
    yoru_arraylist_init(&(__map_ptr)->entries, (__allocator_ptr), YORU_HASHMAP_INITIAL_CAPACITY);                      \
    (__map_ptr)->allocator = (__allocator_ptr);                                                                        \
    yoru_arraylist_init(&(__map_ptr)->keys, (__allocator_ptr), YORU_HASHMAP_INITIAL_CAPACITY);                         \
  } while (0);

#define yoru_hashmap_destroy(__map_ptr)                                                                                \
  do {                                                                                                                 \
    assert((__map_ptr));                                                                                               \
    yoru_arraylist_destroy(&(__map_ptr)->entries);                                                                     \
    for (usize i = 0; i < (__map_ptr)->keys.size; ++i) {                                                               \
      if ((__map_ptr)->keys.items[i].key) free((__map_ptr)->keys.items[i].key);                                        \
    }                                                                                                                  \
    yoru_arraylist_destroy(&(__map_ptr)->keys);                                                                        \
    (__map_ptr)->allocator = NULL;                                                                                     \
  } while (0);

#define yoru_hashmap_grow_if_needed(__map_ptr)                                                                         \
  do {                                                                                                                 \
    assert((__map_ptr));                                                                                               \
    assert((__map_ptr)->allocator);                                                                                    \
    Yoru_Allocator *allocator = (__map_ptr)->allocator;                                                                \
    usize           capacity  = (__map_ptr)->entries.capacity;                                                         \
    usize           size      = (__map_ptr)->entries.size;                                                             \
    f64             load      = (f64)size / (f64)capacity;                                                             \
    /* still within acceptable load -> nothing to do */                                                                \
    if (load < YORU_HASHMAP_LOAD_FACTOR) break;                                                                        \
    usize    new_capacity         = 2 * capacity;                                                                      \
    Yoru_Opt maybe_old_items_copy = yoru_allocator_alloc(allocator, capacity * sizeof((__map_ptr)->entries.items[0])); \
    assert(maybe_old_items_copy.has_value);                                                                            \
    anyptr old_items_copy = maybe_old_items_copy.ptr;                                                                  \
    memcpy(old_items_copy, (__map_ptr)->entries.items, capacity * sizeof((__map_ptr)->entries.items[0]));              \
    yoru_arraylist_resize(&(__map_ptr)->entries, new_capacity);                                                        \
    yoru_arraylist_clear(&(__map_ptr)->entries);                                                                       \
    (__map_ptr)->entries.size = size;                                                                                  \
    for (usize i = 0; i < (__map_ptr)->keys.size; ++i) {                                                               \
      cstr  key       = (__map_ptr)->keys.items[i].key;                                                                \
      usize old_index = (__map_ptr)->keys.items[i].index;                                                              \
      usize hash      = yoru_hash_djb2(key);                                                                           \
      usize new_index = hash % new_capacity;                                                                           \
      /* linear probe to find empty slot */                                                                            \
      while ((__map_ptr)->entries.items[new_index].set)                                                                \
        new_index = (new_index + 1) % new_capacity;                                                                    \
      usize element_size = sizeof((__map_ptr)->entries.items[0]);                                                      \
      /* copy value from old to new index */                                                                           \
      memcpy(                                                                                                          \
          (anyptr)(__map_ptr)->entries.items + element_size * new_index,                                               \
          (anyptr)old_items_copy + element_size * old_index,                                                           \
          element_size);                                                                                               \
      /* update key index */                                                                                           \
      (__map_ptr)->keys.items[i].index = new_index;                                                                    \
    }                                                                                                                  \
    yoru_allocator_dealloc(allocator, old_items_copy);                                                                 \
  } while (0);

#define yoru_hashmap_set(__map_ptr, __key, __value)                                                                    \
  do {                                                                                                                 \
    assert((__map_ptr));                                                                                               \
    assert((__key));                                                                                                   \
    yoru_hashmap_grow_if_needed((__map_ptr));                                                                          \
    usize capacity = (__map_ptr)->entries.capacity;                                                                    \
    usize hash     = yoru_hash_djb2((__key));                                                                          \
    usize index    = hash % capacity;                                                                                  \
    for (usize i = 0; i < capacity; ++i) {                                                                             \
      /* write for NEW key */                                                                                          \
      if (!(__map_ptr)->entries.items[index].set) {                                                                    \
        cstr key_copy                           = strdup((__key));                                                     \
        (__map_ptr)->entries.items[index].key   = key_copy;                                                            \
        (__map_ptr)->entries.items[index].value = (__value);                                                           \
        (__map_ptr)->entries.items[index].set   = true;                                                                \
        yoru_arraylist_append(&((__map_ptr)->keys), ((Yoru_IndexedKey){.key = key_copy, .index = index}));             \
        ++(__map_ptr)->entries.size;                                                                                   \
        break;                                                                                                         \
      }                                                                                                                \
      /* overwrite existing key */                                                                                     \
      if (strcmp((__key), (__map_ptr)->entries.items[index].key) == 0) {                                               \
        (__map_ptr)->entries.items[index].value = (__value);                                                           \
        break;                                                                                                         \
      }                                                                                                                \
      index = (index + 1) % capacity;                                                                                  \
    }                                                                                                                  \
  } while (0)

#define yoru_hashmap_get(__map_ptr, __key, __out_value_ptr)                                                            \
  do {                                                                                                                 \
    assert((__map_ptr));                                                                                               \
    assert((__key));                                                                                                   \
    assert((__out_value_ptr));                                                                                         \
    usize capacity = (__map_ptr)->entries.capacity;                                                                    \
    usize hash     = yoru_hash_djb2((__key));                                                                          \
    usize index    = hash % capacity;                                                                                  \
    for (usize i = 0; i < capacity; ++i) {                                                                             \
      if (!(__map_ptr)->entries.items[index].set) { break; /* key not present */ }                                     \
      if (strcmp((__key), (__map_ptr)->entries.items[index].key) == 0) {                                               \
        *(__out_value_ptr) = (__map_ptr)->entries.items[index].value;                                                  \
        break;                                                                                                         \
      }                                                                                                                \
      index = (index + 1) % capacity;                                                                                  \
    }                                                                                                                  \
  } while (0)

#ifdef YORU_IMPL
usize yoru_hash_djb2(const char *str) {
  usize hash = 5381;
  int   c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c;
  return hash;
}
#endif

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
typedef bool (*Yoru_CharPredicate)(u8 c);

typedef enum { YORU_TRIM_LEFT = 1, YORU_TRIM_RIGHT = 2 } Yoru_TrimOptions;

/// @brief skips `skip` amount of characters from a stringview and returns the
/// new view
Yoru_StringView yoru_stringview_skip(const Yoru_StringView *sv, usize skip);

/// @brief skips characters while the predicate is true and we are within the
/// bounds of the string and returns the new stringview
Yoru_StringView yoru_stringview_skip_while(const Yoru_StringView *sv, Yoru_CharPredicate predicate);

/// @brief returns the first chars (amount = min(`take`, length)) of a
/// stringview
Yoru_StringView yoru_stringview_take(const Yoru_StringView *sv, usize take);

/// @brief returns the first chars of a stringview where the predicate is true
/// or we reach the end of the view as a new view
Yoru_StringView yoru_stringview_take_while(const Yoru_StringView *sv, Yoru_CharPredicate predicate);

/// @brief returns `true` if the string starts with the first `n` chars of
/// `prefix`, else false
bool yoru_stringview_has_prefix(const Yoru_StringView *sv, const char *prefix, usize n);

/// @brief returns `true` if the string contains the first `n` chars of `infix`,
/// else `false`
bool yoru_stringview_has_infix(const Yoru_StringView *sv, const char *infix, usize n);

/// @brief returns `true` if the string ends with the first `n` chars of
/// `suffix`, else false
bool yoru_stringview_has_suffix(const Yoru_StringView *sv, const char *suffix, usize n);

/// @brief returns true if length of the stringview is 0, else false
bool yoru_stringview_is_empty(const Yoru_StringView *sv);

/// @brief returns a new stringview after trimming the whitespaces around the
/// string depending on `trim_options`
/// @note `trim_options` is a bitmap of YORU_TRIM_LEFT and YORU_TRIM_RIGHT
Yoru_StringView yoru_stringview_trim(const Yoru_StringView *sv, Yoru_TrimOptions trim_options);

/// @brief returns a new stringview after trimming if `predicate` is satisfied
/// around the string depending on `trim_options`
/// @note `trim_options` is a bitmap of YORU_TRIM_LEFT and YORU_TRIM_RIGHT. If
/// you just pass 0, the same StringView is returned.
Yoru_StringView
yoru_stringview_trim_while(const Yoru_StringView *sv, Yoru_TrimOptions trim_options, Yoru_CharPredicate predicate);

/// @brief splits a stringview into a dynamic array of stringviews and returns
/// it.
/// @note  please make sure to destroy the dynamic array when you dont need it
/// anymore
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
    if (!predicate(c)) break;
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
    if (!predicate(c)) break;
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
    if (i + n < sv->length) {
      if (memcmp(sv->data + i, infix, n) == 0) return true;
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

Yoru_StringView
__yoru_stringview_trim_core(const Yoru_StringView *sv, Yoru_TrimOptions trim_options, Yoru_CharPredicate predicate);

static inline bool __yoru_isspace(u8 c) {
  return isspace((int)c);
}

Yoru_StringView yoru_stringview_trim(const Yoru_StringView *sv, Yoru_TrimOptions trim_options) {
  return __yoru_stringview_trim_core(sv, trim_options, __yoru_isspace);
}

Yoru_StringView
yoru_stringview_trim_while(const Yoru_StringView *sv, Yoru_TrimOptions trim_options, Yoru_CharPredicate predicate) {
  return __yoru_stringview_trim_core(sv, trim_options, predicate);
}

Yoru_StringView
__yoru_stringview_trim_core(const Yoru_StringView *sv, Yoru_TrimOptions trim_options, Yoru_CharPredicate predicate) {
  assert(sv);
  assert(predicate);

  usize start = 0;
  usize end   = sv->length; // exclusive

  if (trim_options & YORU_TRIM_LEFT) {
    while (start < end && predicate(sv->data[start])) {
      ++start;
    }
  }

  if (trim_options & YORU_TRIM_RIGHT) {
    while (end > start && predicate(sv->data[end - 1])) {
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

  max_split_count              = max_split_count == USIZE_MAX ? YORU_ARRAYLIST_INITIAL_CAPACITY : max_split_count;
  Yoru_StringViews stringviews = {0};
  yoru_arraylist_init(&stringviews, allocator, max_split_count);

  usize curr_start = 0;
  for (usize i = 0; i < sv->length && stringviews.size < max_split_count; ++i) {
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
  if (curr_start < sv->length && stringviews.size < max_split_count) {
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

/// @brief Creates an empty string of specific length. If `initial_value` is
/// NULL then the string is just a zero string.
bool yoru_string_make(Yoru_Allocator *allocator, usize length, const char *initial_value, Yoru_String *out_string);

/// @brief Creates a DEEP-copy of a string
bool yoru_string_copy(Yoru_Allocator *allocator, Yoru_String *src, Yoru_String *dest);

/// @brief Deallocates the string using the allocator that owns it
void yoru_string_destroy(Yoru_String *s);

/// @brief Creates a new string which is the substring of a given string
/// @note Changing values in the substring also changes values in the "complete"
/// string as it is just a small window/view of the original string
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
     - add a `yoru_stringbuilder_append_format(format, ...)` instead of char and
   cstr func
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

/// @brief Creates a string from the stringbuider, copying the data of the
/// stringbuilder
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

/// @brief reads an entire file and returns the content in a string
Yoru_String yoru_file_read(Yoru_Allocator *allocator, const char *filepath);

/// @brief reads a maximum of `max_bytes` with offset `offset_bytes` from a file
/// and returns content in a string
Yoru_String yoru_file_read_exact(Yoru_Allocator *allocator, const char *filepath, usize offset_bytes, usize max_bytes);

/// @brief writes `nbytes` of `bytes` to position `offset` file at path `filepath`
/// @note if `offset` is greater than the size of the file it will just append them instead
/// returns `true` on success, else `false`
bool yoru_file_write_exact(const char *filepath, const u8 *bytes, usize nbytes, usize offset);

/// @brief writes `nbytes` of `bytes` to end of file at path `filepath`
/// returns `true` on success, else `false`
bool yoru_file_append_exact(const char *filepath, const u8 *bytes, usize nbytes);

/// @brief returns the file's size in bytes
usize yoru_file_get_size(const char *filepath);

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
  (void)fread((anyptr)res.data, sizeof(u8), res.length, file);
  fclose(file);
  return res;

cleanup:
  if (file) fclose(file);
  res.data = NULL;
  return res;
}

bool yoru_file_write_exact(const char *filepath, const u8 *bytes, usize nbytes, usize offset) {
  assert(filepath);
  assert(bytes);

  usize       file_size = yoru_file_get_size(filepath);
  const char *mode      = "w";
  if (offset >= file_size) {
    offset = file_size;
    mode   = "a";
  }

  FILE *file = fopen(filepath, mode);
  if (!file) return false;

  usize written = fwrite((anyptr)bytes, sizeof(u8), nbytes, file);
  fclose(file);
  return written == nbytes;
}

bool yoru_file_append_exact(const char *filepath, const u8 *bytes, usize nbytes) {
  assert(filepath);
  assert(bytes);
  usize file_size = yoru_file_get_size(filepath);
  return yoru_file_write_exact(filepath, bytes, nbytes, file_size);
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

/* ============================================================
   MODULE: Vectors

   NOTE: There are Vec2, Vec3, Vec4 for f64 predefined with their
   functions. If you are using another typedef for the vectors
   look at how the funcs are defined (usually in a core func) and
   really short. In the future we want to provide a generic interface
   for other funcs too
   
   TODO:   - [] add SIMD optimizations
                -> Do I really want manual SIMD optimizations or should
                   I just trust the C compiler with O3?
   ============================================================ */

#define Yoru_Vec_T(__T, __N)                                                                                           \
  struct {                                                                                                             \
    __T elements[__N];                                                                                                 \
  }

#define yoru_vec_make(...)                                                                                             \
  {                                                                                                                    \
    .elements = { __VA_ARGS__ }                                                                                        \
  }

typedef Yoru_Vec_T(f64, 2) Yoru_Vec2_F64;
typedef Yoru_Vec_T(f64, 3) Yoru_Vec3_F64;
typedef Yoru_Vec_T(f64, 4) Yoru_Vec4_F64;

typedef enum {
  YORU_VEC_ERR_OK                    = (1 << 0),
  YORU_VEC_ERR_NULL                  = (1 << 1),
  YORU_VEC_ERR_MISMATCHED_DIMENSIONS = (1 << 2),
} Yoru_VecErr;

Yoru_VecErr yoru_vec_add(usize n, const f64 v1[static n], const f64 v2[static n], f64 out_v[static n]);
Yoru_VecErr yoru_vec2_add(const Yoru_Vec2_F64 *v1, const Yoru_Vec2_F64 *v2, Yoru_Vec2_F64 *out_v);
Yoru_VecErr yoru_vec3_add(const Yoru_Vec3_F64 *v1, const Yoru_Vec3_F64 *v2, Yoru_Vec3_F64 *out_v);
Yoru_VecErr yoru_vec4_add(const Yoru_Vec4_F64 *v1, const Yoru_Vec4_F64 *v2, Yoru_Vec4_F64 *out_v);

Yoru_VecErr yoru_vec_sub(usize n, const f64 v1[static n], const f64 v2[static n], f64 out_v[static n]);
Yoru_VecErr yoru_vec2_sub(const Yoru_Vec2_F64 *v1, const Yoru_Vec2_F64 *v2, Yoru_Vec2_F64 *out_v);
Yoru_VecErr yoru_vec3_sub(const Yoru_Vec3_F64 *v1, const Yoru_Vec3_F64 *v2, Yoru_Vec3_F64 *out_v);
Yoru_VecErr yoru_vec4_sub(const Yoru_Vec4_F64 *v1, const Yoru_Vec4_F64 *v2, Yoru_Vec4_F64 *out_v);

Yoru_VecErr yoru_vec_mul(usize n, const f64 v1[static n], const f64 v2[static n], f64 out_v[static n]);
Yoru_VecErr yoru_vec2_mul(const Yoru_Vec2_F64 *v1, const Yoru_Vec2_F64 *v2, Yoru_Vec2_F64 *out_v);
Yoru_VecErr yoru_vec3_mul(const Yoru_Vec3_F64 *v1, const Yoru_Vec3_F64 *v2, Yoru_Vec3_F64 *out_v);
Yoru_VecErr yoru_vec4_mul(const Yoru_Vec4_F64 *v1, const Yoru_Vec4_F64 *v2, Yoru_Vec4_F64 *out_v);

Yoru_VecErr yoru_vec_div(usize n, const f64 v1[static n], const f64 v2[static n], f64 out_v[static n]);
Yoru_VecErr yoru_vec2_div(const Yoru_Vec2_F64 *v1, const Yoru_Vec2_F64 *v2, Yoru_Vec2_F64 *out_v);
Yoru_VecErr yoru_vec3_div(const Yoru_Vec3_F64 *v1, const Yoru_Vec3_F64 *v2, Yoru_Vec3_F64 *out_v);
Yoru_VecErr yoru_vec4_div(const Yoru_Vec4_F64 *v1, const Yoru_Vec4_F64 *v2, Yoru_Vec4_F64 *out_v);

Yoru_VecErr yoru_vec_dot(usize n, const f64 v1[static n], const f64 v2[static n], f64 *out);
Yoru_VecErr yoru_vec2_dot(const Yoru_Vec2_F64 *v1, const Yoru_Vec2_F64 *v2, f64 *out_v);
Yoru_VecErr yoru_vec3_dot(const Yoru_Vec3_F64 *v1, const Yoru_Vec3_F64 *v2, f64 *out_v);
Yoru_VecErr yoru_vec4_dot(const Yoru_Vec4_F64 *v1, const Yoru_Vec4_F64 *v2, f64 *out_v);

Yoru_VecErr yoru_vec_scale(usize n, const f64 v[static n], f64 scalar, f64 out_v[static n]);
Yoru_VecErr yoru_vec2_scale(const Yoru_Vec2_F64 *v, f64 scalar, Yoru_Vec2_F64 *out_v);
Yoru_VecErr yoru_vec3_scale(const Yoru_Vec3_F64 *v, f64 scalar, Yoru_Vec3_F64 *out_v);
Yoru_VecErr yoru_vec4_scale(const Yoru_Vec4_F64 *v, f64 scalar, Yoru_Vec4_F64 *out_v);

Yoru_VecErr yoru_vec_length_squared(usize n, const f64 v[static n], f64 *out);
Yoru_VecErr yoru_vec2_length_squared(const Yoru_Vec2_F64 *v, f64 *out_length);
Yoru_VecErr yoru_vec3_length_squared(const Yoru_Vec3_F64 *v, f64 *out_length);
Yoru_VecErr yoru_vec4_length_squared(const Yoru_Vec4_F64 *v, f64 *out_length);

Yoru_VecErr yoru_vec_max_between(usize n, const f64 v1[static n], const f64 v2[static n], f64 out_v[static n]);
Yoru_VecErr yoru_vec2_max_between(const Yoru_Vec2_F64 *v1, const Yoru_Vec2_F64 *v2, Yoru_Vec2_F64 *out_v);
Yoru_VecErr yoru_vec3_max_between(const Yoru_Vec3_F64 *v1, const Yoru_Vec3_F64 *v2, Yoru_Vec3_F64 *out_v);
Yoru_VecErr yoru_vec4_max_between(const Yoru_Vec4_F64 *v1, const Yoru_Vec4_F64 *v2, Yoru_Vec4_F64 *out_v);

Yoru_VecErr yoru_vec_min_between(usize n, const f64 v1[static n], const f64 v2[static n], f64 out_v[static n]);
Yoru_VecErr yoru_vec2_min_between(const Yoru_Vec2_F64 *v1, const Yoru_Vec2_F64 *v2, Yoru_Vec2_F64 *out_v);
Yoru_VecErr yoru_vec3_min_between(const Yoru_Vec3_F64 *v1, const Yoru_Vec3_F64 *v2, Yoru_Vec3_F64 *out_v);
Yoru_VecErr yoru_vec4_min_between(const Yoru_Vec4_F64 *v1, const Yoru_Vec4_F64 *v2, Yoru_Vec4_F64 *out_v);

Yoru_VecErr yoru_vec3_cross(const Yoru_Vec3_F64 *v1, const Yoru_Vec3_F64 *v2, Yoru_Vec3_F64 *out_v);

#if defined(__AVX__) && defined(YORU_USE_SIMD)
#  include <immintrin.h>
#endif

#ifdef YORU_IMPL
Yoru_VecErr yoru_vec_add(usize n, const f64 v1[static n], const f64 v2[static n], f64 out_v[static n]) {
  if (n == 0 || !v1 || !v2 || !out_v) return YORU_VEC_ERR_NULL;

#  if defined(__AVX__) && defined(YORU_USE_SIMD)
  usize i = 0;
  if (n > 3) {
    usize batch_size = 4;
    for (; i + batch_size <= n; i += batch_size) {
      __m256d a = _mm256_loadu_pd(&v1[i]);
      __m256d b = _mm256_loadu_pd(&v2[i]);
      _mm256_storeu_pd(&out_v[i], _mm256_add_pd(a, b));
    }
  }

  for (; i < n; ++i)
    out_v[i] = v1[i] + v2[i];
#  else // fallback scalar
  for (usize i = 0; i < n; ++i)
    out_v[i] = v1[i] + v2[i];
#  endif
  return YORU_VEC_ERR_OK;
}

Yoru_VecErr yoru_vec2_add(const Yoru_Vec2_F64 *v1, const Yoru_Vec2_F64 *v2, Yoru_Vec2_F64 *out_v) {
  return yoru_vec_add(2, (const f64 *)v1->elements, (const f64 *)v2->elements, out_v->elements);
}

Yoru_VecErr yoru_vec3_add(const Yoru_Vec3_F64 *v1, const Yoru_Vec3_F64 *v2, Yoru_Vec3_F64 *out_v) {
  return yoru_vec_add(3, (const f64 *)v1->elements, (const f64 *)v2->elements, out_v->elements);
}

Yoru_VecErr yoru_vec4_add(const Yoru_Vec4_F64 *v1, const Yoru_Vec4_F64 *v2, Yoru_Vec4_F64 *out_v) {
  return yoru_vec_add(4, (const f64 *)v1->elements, (const f64 *)v2->elements, out_v->elements);
}

Yoru_VecErr yoru_vec_sub(usize n, const f64 v1[static n], const f64 v2[static n], f64 out_v[static n]) {
  if (n == 0 || !v1 || !v2 || !out_v) return YORU_VEC_ERR_NULL;

#  if defined(__AVX__) && defined(YORU_USE_SIMD)
  usize i = 0;
  if (n > 3) {
    usize batch_size = 4;
    for (; i + batch_size <= n; i += batch_size) {
      __m256d a = _mm256_loadu_pd(&v1[i]);
      __m256d b = _mm256_loadu_pd(&v2[i]);
      _mm256_storeu_pd(&out_v[i], _mm256_sub_pd(a, b));
    }
  }

  for (; i < n; ++i)
    out_v[i] = v1[i] - v2[i];
#  else // fallback scalar
  for (usize i = 0; i < n; ++i)
    out_v[i] = v1[i] - v2[i];
#  endif
  return YORU_VEC_ERR_OK;
}

Yoru_VecErr yoru_vec2_sub(const Yoru_Vec2_F64 *v1, const Yoru_Vec2_F64 *v2, Yoru_Vec2_F64 *out_v) {
  return yoru_vec_sub(2, v1->elements, v2->elements, out_v->elements);
}

Yoru_VecErr yoru_vec3_sub(const Yoru_Vec3_F64 *v1, const Yoru_Vec3_F64 *v2, Yoru_Vec3_F64 *out_v) {
  return yoru_vec_sub(3, v1->elements, v2->elements, out_v->elements);
}

Yoru_VecErr yoru_vec4_sub(const Yoru_Vec4_F64 *v1, const Yoru_Vec4_F64 *v2, Yoru_Vec4_F64 *out_v) {
  return yoru_vec_sub(4, v1->elements, v2->elements, out_v->elements);
}

Yoru_VecErr yoru_vec_mul(usize n, const f64 v1[static n], const f64 v2[static n], f64 out_v[static n]) {
  if (n == 0 || !v1 || !v2 || !out_v) return YORU_VEC_ERR_NULL;

#  if defined(__AVX__) && defined(YORU_USE_SIMD)
  usize i = 0;
  if (n > 3) {
    usize batch_size = 4;
    for (; i + batch_size <= n; i += batch_size) {
      __m256d a = _mm256_loadu_pd(&v1[i]);
      __m256d b = _mm256_loadu_pd(&v2[i]);
      _mm256_storeu_pd(&out_v[i], _mm256_mul_pd(a, b));
    }
  }

  for (; i < n; ++i)
    out_v[i] = v1[i] * v2[i];
#  else // fallback scalar
  for (usize i = 0; i < n; ++i)
    out_v[i] = v1[i] * v2[i];
#  endif
  return YORU_VEC_ERR_OK;
}

Yoru_VecErr yoru_vec2_mul(const Yoru_Vec2_F64 *v1, const Yoru_Vec2_F64 *v2, Yoru_Vec2_F64 *out_v) {
  return yoru_vec_mul(2, v1->elements, v2->elements, out_v->elements);
}

Yoru_VecErr yoru_vec3_mul(const Yoru_Vec3_F64 *v1, const Yoru_Vec3_F64 *v2, Yoru_Vec3_F64 *out_v) {
  return yoru_vec_mul(3, v1->elements, v2->elements, out_v->elements);
}

Yoru_VecErr yoru_vec4_mul(const Yoru_Vec4_F64 *v1, const Yoru_Vec4_F64 *v2, Yoru_Vec4_F64 *out_v) {
  return yoru_vec_mul(4, v1->elements, v2->elements, out_v->elements);
}

Yoru_VecErr yoru_vec_div(usize n, const f64 v1[static n], const f64 v2[static n], f64 out_v[static n]) {
  if (n == 0 || !v1 || !v2 || !out_v) return YORU_VEC_ERR_NULL;

#  if defined(__AVX__) && defined(YORU_USE_SIMD)
  usize i = 0;
  if (n > 3) {
    usize batch_size = 4;
    for (; i + batch_size <= n; i += batch_size) {
      __m256d a = _mm256_loadu_pd(&v1[i]);
      __m256d b = _mm256_loadu_pd(&v2[i]);
      _mm256_storeu_pd(&out_v[i], _mm256_div_pd(a, b));
    }
  }

  for (; i < n; ++i)
    out_v[i] = v1[i] / v2[i];
#  else // fallback scalar
  for (usize i = 0; i < n; ++i)
    out_v[i] = v1[i] / v2[i];
#  endif
  return YORU_VEC_ERR_OK;
}

Yoru_VecErr yoru_vec2_div(const Yoru_Vec2_F64 *v1, const Yoru_Vec2_F64 *v2, Yoru_Vec2_F64 *out_v) {
  return yoru_vec_div(2, v1->elements, v2->elements, out_v->elements);
}

Yoru_VecErr yoru_vec3_div(const Yoru_Vec3_F64 *v1, const Yoru_Vec3_F64 *v2, Yoru_Vec3_F64 *out_v) {
  return yoru_vec_div(3, v1->elements, v2->elements, out_v->elements);
}

Yoru_VecErr yoru_vec4_div(const Yoru_Vec4_F64 *v1, const Yoru_Vec4_F64 *v2, Yoru_Vec4_F64 *out_v) {
  return yoru_vec_div(4, v1->elements, v2->elements, out_v->elements);
}

Yoru_VecErr yoru_vec_dot(usize n, const f64 v1[static n], const f64 v2[static n], f64 *out) {
  if (n == 0 || !v1 || !v2 || !out) return YORU_VEC_ERR_NULL;
  *out = 0;
  for (usize i = 0; i < n; ++i)
    *out += v1[i] * v2[i];
  return YORU_VEC_ERR_OK;
}

Yoru_VecErr yoru_vec2_dot(const Yoru_Vec2_F64 *v1, const Yoru_Vec2_F64 *v2, f64 *out) {
  return yoru_vec_dot(2, v1->elements, v2->elements, out);
}

Yoru_VecErr yoru_vec3_dot(const Yoru_Vec3_F64 *v1, const Yoru_Vec3_F64 *v2, f64 *out) {
  return yoru_vec_dot(3, v1->elements, v2->elements, out);
}

Yoru_VecErr yoru_vec4_dot(const Yoru_Vec4_F64 *v1, const Yoru_Vec4_F64 *v2, f64 *out) {
  return yoru_vec_dot(4, v1->elements, v2->elements, out);
}

Yoru_VecErr yoru_vec_scale(usize n, const f64 v[static n], f64 scalar, f64 out_v[static n]) {
  if (n == 0 || !v || !out_v) return YORU_VEC_ERR_NULL;
  for (usize i = 0; i < n; ++i)
    out_v[i] = v[i] * scalar;
  return YORU_VEC_ERR_OK;
}

Yoru_VecErr yoru_vec2_scale(const Yoru_Vec2_F64 *v, f64 scalar, Yoru_Vec2_F64 *out_v) {
  return yoru_vec_scale(2, v->elements, scalar, out_v->elements);
}

Yoru_VecErr yoru_vec3_scale(const Yoru_Vec3_F64 *v, f64 scalar, Yoru_Vec3_F64 *out_v) {
  return yoru_vec_scale(3, v->elements, scalar, out_v->elements);
}

Yoru_VecErr yoru_vec4_scale(const Yoru_Vec4_F64 *v, f64 scalar, Yoru_Vec4_F64 *out_v) {
  return yoru_vec_scale(4, v->elements, scalar, out_v->elements);
}

Yoru_VecErr yoru_vec_length_squared(usize n, const f64 v[static n], f64 *out) {
  if (n == 0 || !v || !out) return YORU_VEC_ERR_NULL;
  *out = 0;
  for (usize i = 0; i < n; ++i)
    *out += v[i] * v[i];
  return YORU_VEC_ERR_OK;
}

Yoru_VecErr yoru_vec2_length_squared(const Yoru_Vec2_F64 *v, f64 *out_length) {
  return yoru_vec_length_squared(2, v->elements, out_length);
}

Yoru_VecErr yoru_vec3_length_squared(const Yoru_Vec3_F64 *v, f64 *out_length) {
  return yoru_vec_length_squared(3, v->elements, out_length);
}

Yoru_VecErr yoru_vec4_length_squared(const Yoru_Vec4_F64 *v, f64 *out_length) {
  return yoru_vec_length_squared(4, v->elements, out_length);
}

Yoru_VecErr yoru_vec_max_between(usize n, const f64 v1[static n], const f64 v2[static n], f64 out_v[static n]) {
  if (n == 0 || !v1 || !v2 || !out_v) return YORU_VEC_ERR_NULL;
  for (usize i = 0; i < n; ++i)
    out_v[i] = YORU_MAX(v1[i], v2[i]);
  return YORU_VEC_ERR_OK;
}

Yoru_VecErr yoru_vec2_max_between(const Yoru_Vec2_F64 *v1, const Yoru_Vec2_F64 *v2, Yoru_Vec2_F64 *out_v) {
  return yoru_vec_max_between(2, v1->elements, v2->elements, out_v->elements);
}

Yoru_VecErr yoru_vec3_max_between(const Yoru_Vec3_F64 *v1, const Yoru_Vec3_F64 *v2, Yoru_Vec3_F64 *out_v) {
  return yoru_vec_max_between(3, v1->elements, v2->elements, out_v->elements);
}

Yoru_VecErr yoru_vec4_max_between(const Yoru_Vec4_F64 *v1, const Yoru_Vec4_F64 *v2, Yoru_Vec4_F64 *out_v) {
  return yoru_vec_max_between(4, v1->elements, v2->elements, out_v->elements);
}

Yoru_VecErr yoru_vec_min_between(usize n, const f64 v1[static n], const f64 v2[static n], f64 out_v[static n]) {
  if (n == 0 || !v1 || !v2 || !out_v) return YORU_VEC_ERR_NULL;
  for (usize i = 0; i < n; ++i)
    out_v[i] = YORU_MIN(v1[i], v2[i]);
  return YORU_VEC_ERR_OK;
}

Yoru_VecErr yoru_vec2_min_between(const Yoru_Vec2_F64 *v1, const Yoru_Vec2_F64 *v2, Yoru_Vec2_F64 *out_v) {
  return yoru_vec_min_between(2, v1->elements, v2->elements, out_v->elements);
}

Yoru_VecErr yoru_vec3_min_between(const Yoru_Vec3_F64 *v1, const Yoru_Vec3_F64 *v2, Yoru_Vec3_F64 *out_v) {
  return yoru_vec_min_between(3, v1->elements, v2->elements, out_v->elements);
}

Yoru_VecErr yoru_vec4_min_between(const Yoru_Vec4_F64 *v1, const Yoru_Vec4_F64 *v2, Yoru_Vec4_F64 *out_v) {
  return yoru_vec_min_between(4, v1->elements, v2->elements, out_v->elements);
}

Yoru_VecErr yoru_vec3_cross(const Yoru_Vec3_F64 *v1, const Yoru_Vec3_F64 *v2, Yoru_Vec3_F64 *out_v) {
  if (!v1 || !v2 || !out_v) return YORU_VEC_ERR_NULL;
  out_v->elements[0] = v1->elements[1] * v2->elements[2] - v1->elements[2] * v2->elements[1];
  out_v->elements[1] = v1->elements[2] * v2->elements[0] - v1->elements[0] * v2->elements[2];
  out_v->elements[2] = v1->elements[0] * v2->elements[1] - v1->elements[1] * v2->elements[0];
  return YORU_VEC_ERR_OK;
}

#endif // YORU_IMPL

/* ============================================================
   MODULE: Matrices
   TODO:
           - add SIMD optimizations
           - add operations
   ============================================================ */

// column-major matrix
#define Yoru_Mat_T(__T, __ROWS, __COLS)                                                                                \
  struct {                                                                                                             \
    Yoru_Vec_T(__T, __ROWS) cols[__COLS];                                                                              \
    usize nrows;                                                                                                       \
    usize ncols;                                                                                                       \
  }

typedef Yoru_Mat_T(f64, 2, 2) Yoru_Mat2x2_F64;
typedef Yoru_Mat_T(f64, 3, 3) Yoru_Mat3x3_F64;
typedef Yoru_Mat_T(f64, 4, 4) Yoru_Mat4x4_F64;

#endif // __YORU_H__
