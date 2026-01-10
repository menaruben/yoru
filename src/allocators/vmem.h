#ifndef __YORU_VMEM_H__
#define __YORU_VMEM_H__

#include "../types/types.h"
#include <assert.h>

typedef struct Yoru_Vmem_Ctx {
  anyptr base;
  usize  commit_pos;
  usize  addr_space_size;
} Yoru_Vmem_Ctx;

#ifdef __linux__
#  include <sys/mman.h>
#  include <unistd.h>
#else
#  error "platform not supported yet"
#endif

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

#endif
#endif
