#ifndef __YORU_PLATFORMS_H__
#define __YORU_PLATFORMS_H__

#if defined(_WIN32) || defined(_WIN64)
#define YORU_PLATFORM_WINDOWS 1
#else
#define YORU_PLATFORM_WINDOWS 0
#endif

#if defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(_POSIX_VERSION) || defined(_POSIX_C_SOURCE)
#define YORU_PLATFORM_UNIX 1
#else
#define YORU_PLATFORM_UNIX 0
#endif

#endif