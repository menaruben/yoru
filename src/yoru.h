#ifndef __YORU_H__
#define __YORU_H__

// use YORU_IMPLEMENTATION to include the implementation of the library

#include "./inttypes/yoru_inttypes.h"
#include "./collections/yoru_collections.h"
#include "./allocators/yoru_allocators.h"
#include "./math/yoru_math.h"
#include "./asserts/yoru_asserts.h"
#include "./utils/yoru_utils.h"
#include "./strings/yoru_strings.h"
#include "./testing/yoru_tests.h"
#include "./futures/yoru_futures.h"
#include "cli/yoru_cli.h"
#include "yoru_platforms.h"

// some yoru features are only available in strict mode
#ifdef YORU_STRICT
#define YORU_EXIT_ON_ARENA_REALLOC
#endif

#endif