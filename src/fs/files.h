#ifndef __YORU_FILES_H__
#define __YORU_FILES_H__


#include "../strings/stringview.h"
#include "../allocators/allocator.h"

Yoru_String yoru_read_file(Yoru_Allocator *allocator, const char *filepath);

#ifdef YORU_IMPL
#include "../contracts/contracts.h"
#include <stdio.h>

Yoru_String yoru_read_file(Yoru_Allocator *allocator, const char *filepath) {
  Yoru_Guard_NotNull(allocator);
  YOru_Guard_NotNull(filepath);

  Yoru_String res = {0};
  
  FILE *file = fopen(filepath, "r");
  if (!file) goto cleanup;

  fseek(file, 0, SEEK_END);
  usize size = ftell(file);
  
cleanup:
  if (file) fclose(file);
  return res;
}

#endif
#endif // __YORU_FILES_H__
