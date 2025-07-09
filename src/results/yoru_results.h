#ifndef __YORU_RESULTS_H__
#define __YORU_RESULTS_H__

typedef enum
{
    YORU_OK,
    YORU_ERR_ALLOC,
    YORU_ERR_FREE,
    YORU_ERR_REALLOC,
    YORU_ERR_OUT_OF_BOUNDS,
    YORU_ERR_ARGUMENT_NULL,
    YORU_ERR_ARGUMENT_INVALID,
} Yoru_Error_t;

#define Yoru_Result(TValue, TError) \
    struct Yoru_Result_t            \
    {                               \
        TValue value;               \
        TError err;                 \
    }

typedef Yoru_Result(void *, Yoru_Error_t) Yoru_Result_t;

#endif