#ifndef __YORU_FUNCTIONS_H__
#define __YORU_FUNCTIONS_H__

#define FUNC(ret_t, name, ...) \
    ret_t (*name)(__VA_ARGS__)

#endif