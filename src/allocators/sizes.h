#ifndef __YORU_MEM_SIZES_H__
#define __YORU_MEM_SIZES_H__

#include "../types/types.h"

#define YORU_B(_n) ((usize)1 * (_n))

#define YORU_KB(_n) ((_n) * YORU_B(1000))  // 1 KB = 1000 bytes
#define YORU_KiB(_n) ((_n) * YORU_B(1024)) // 1 KiB = 1024 bytes

#define YORU_MB(_n) ((_n) * YORU_KB(1000))   // 1 MB = 1000 KB
#define YORU_MiB(_n) ((_n) * YORU_KiB(1024)) // 1 MiB = 1024 KiB

#define YORU_GB(_n) ((_n) * YORU_MB(1000))   // 1 GB = 1000 MB
#define YORU_GiB(_n) ((_n) * YORU_MiB(1024)) // 1 GiB = 1024 MiB

#endif // __YORU_MEM_SIZES_H__
