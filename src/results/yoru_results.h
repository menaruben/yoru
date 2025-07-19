#ifndef __YORU_RESULTS_H__
#define __YORU_RESULTS_H__

#include "../utils/yoru_utils.h"

typedef enum Yoru_ErrorType_t
{
    YORU_OK,
    YORU_ERR_ALLOC,
    YORU_ERR_FREE,
    YORU_ERR_REALLOC,
    YORU_ERR_OUT_OF_MEMORY,
    YORU_ERR_KEY_NOT_FOUND,
    YORU_ERR_OUT_OF_BOUNDS,
    YORU_ERR_ARGUMENT_NULL,
    YORU_ERR_ARGUMENT_INVALID,
    YORU_ERR_UNINITIALIZED,
} Yoru_ErrorType_t;

typedef struct Yoru_Error_t
{
    Yoru_ErrorType_t type;
    const char *message;
} Yoru_Error_t;

const char *yoru_error_to_string(Yoru_ErrorType_t err);

#ifdef YORU_IMPLEMENTATION

const char *yoru_error_to_string(Yoru_ErrorType_t err)
{
    switch (err)
    {
    case YORU_OK:
        return YORU_NAMEOF(YORU_OK);
    case YORU_ERR_ALLOC:
        return YORU_NAMEOF(YORU_ERR_ALLOC);
    case YORU_ERR_FREE:
        return YORU_NAMEOF(YORU_ERR_FREE);
    case YORU_ERR_REALLOC:
        return YORU_NAMEOF(YORU_ERR_REALLOC);
    case YORU_ERR_KEY_NOT_FOUND:
        return YORU_NAMEOF(YORU_ERR_KEY_NOT_FOUND);
    case YORU_ERR_OUT_OF_BOUNDS:
        return YORU_NAMEOF(YORU_ERR_OUT_OF_BOUNDS);
    case YORU_ERR_ARGUMENT_NULL:
        return YORU_NAMEOF(YORU_ERR_ARGUMENT_NULL);
    case YORU_ERR_ARGUMENT_INVALID:
        return YORU_NAMEOF(YORU_ERR_ARGUMENT_INVALID);
    default:
        return "Unknown error";
    }
}

#endif

#endif