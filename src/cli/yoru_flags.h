#ifndef __YORU_FLAGS_H__
#define __YORU_FLAGS_H__

#include <stdbool.h>
#include <stddef.h>
#include "../collections/yoru_trie.h"
#include "../inttypes/yoru_inttypes.h"
#include "../allocators/yoru_allocators.h"
#include "../yoru_defs.h"
#include "../results/yoru_results.h"
#include "string.h"
#include "../utils/yoru_utils.h"

typedef union
{
    bool boolean;
    i64 integer;
    u64 uinteger;
    f64 floating;
    const char *string;
} Yoru_Flag_Value_t;

typedef enum
{
    YORU_FLAG_TYPE_BOOL,
    YORU_FLAG_TYPE_INT,
    YORU_FLAG_TYPE_UINT,
    YORU_FLAG_TYPE_FLOAT,
    YORU_FLAG_TYPE_STRING,
} Yoru_Flag_Type_t;

typedef struct
{
    const char *short_name;  // e.g. "-h"
    const char *name;        // e.g. "--help"
    const char *description; // e.g. "Show help information"
    bool is_required;
    bool is_set;
    Yoru_Flag_Value_t value;
    Yoru_Flag_Type_t type;
} Yoru_Flag_t;

#define yoru_flag_new(short_name, name, description, is_required, type) \
    ((Yoru_Flag_t){short_name, name, description, is_required, type, false})

typedef enum
{
    YORU_FLAGPARSE_ERROR_OK,
    YORU_FLAGPARSE_ERROR_REQUIRED_FLAG_NOT_SET,
    YORU_FLAGPARSE_ERROR_UNKNOWN_FLAG,
    YORU_FLAGPARSE_ERROR_INVALID_VALUE_FOR_FLAG_TYPE,
    YORU_FLAGPARSE_ERROR_FLAGSET_CREATION_FAILED,
    YORU_FLAGPARSE_ERROR_FLAG_HAS_NO_NAME,
    YORU_FLAGPARSE_ERROR_FLAG_REQUIRES_VALUE,
} Yoru_FlagParse_Error_t;

typedef Yoru_Result(Yoru_Flag_t *, Yoru_FlagParse_Error_t) Yoru_FlagParse_Result_t;

YORU_API const char *yoru_flagparse_error_to_string(Yoru_FlagParse_Error_t err);

YORU_API void yoru_print_help(
    Yoru_Flag_t *flags,
    size_t flag_count,
    const char *program_name,
    const char *description);

YORU_API Yoru_FlagParse_Result_t yoru_flagset_parse(
    Yoru_Allocator_t *allocator,
    int argc,
    char **argv,
    Yoru_Flag_t *flags,
    size_t flag_count);

YORU_API void yoru_print_flag(Yoru_Flag_t *flag);

YORU_HELPER Yoru_Error_t yoru_flagset_init(
    TrieNode_t *trie,
    Yoru_Flag_t *flags,
    size_t flag_count,
    Yoru_Allocator_t *allocator);

YORU_HELPER bool yoru_all_required_flags_set(
    Yoru_Flag_t *flags,
    size_t flag_count);

#ifdef YORU_IMPLEMENTATION

YORU_API const char *yoru_flagparse_error_to_string(Yoru_FlagParse_Error_t err)
{
    switch (err)
    {
    case YORU_FLAGPARSE_ERROR_OK:
        return YORU_NAMEOF(YORU_FLAGPARSE_ERROR_OK);
    case YORU_FLAGPARSE_ERROR_REQUIRED_FLAG_NOT_SET:
        return YORU_NAMEOF(YORU_FLAGPARSE_ERROR_REQUIRED_FLAG_NOT_SET);
    case YORU_FLAGPARSE_ERROR_UNKNOWN_FLAG:
        return YORU_NAMEOF(YORU_FLAGPARSE_ERROR_UNKNOWN_FLAG);
    case YORU_FLAGPARSE_ERROR_INVALID_VALUE_FOR_FLAG_TYPE:
        return YORU_NAMEOF(YORU_FLAGPARSE_ERROR_INVALID_VALUE_FOR_FLAG_TYPE);
    case YORU_FLAGPARSE_ERROR_FLAGSET_CREATION_FAILED:
        return YORU_NAMEOF(YORU_FLAGPARSE_ERROR_FLAGSET_CREATION_FAILED);
    case YORU_FLAGPARSE_ERROR_FLAG_HAS_NO_NAME:
        return YORU_NAMEOF(YORU_FLAGPARSE_ERROR_FLAG_HAS_NO_NAME);
    case YORU_FLAGPARSE_ERROR_FLAG_REQUIRES_VALUE:
        return YORU_NAMEOF(YORU_FLAGPARSE_ERROR_FLAG_REQUIRES_VALUE);
    default:
        return "Unknown error";
    }
}

YORU_API void yoru_print_help(
    Yoru_Flag_t *flags,
    size_t flag_count,
    const char *program_name,
    const char *description)
{
    if (!program_name)
    {
        program_name = "<program>";
    }

    if (!description)
    {
        description = "";
    }

    printf("Usage: %s [options]\n\n", program_name);
    printf("%s\n\n", description);

    if (flag_count > 0)
    {
        printf("Options:\n");
    }

    for (size_t i = 0; i < flag_count; ++i)
    {
        printf("  %s, %s: %s %s\n",
               flags[i].short_name ? flags[i].short_name : "<NONE>",
               flags[i].name ? flags[i].name : "<NONE>",
               flags[i].description,
               flags[i].is_required ? "(required)" : "(optional)");
    }
}

YORU_API Yoru_FlagParse_Result_t yoru_flagset_parse(
    Yoru_Allocator_t *allocator,
    int argc,
    char **argv,
    Yoru_Flag_t *flags,
    size_t flag_count)
{
    Yoru_Error_t err;
    Yoru_Result_t res;
    TrieNode_t *flagset = trie_new(allocator);

    err = yoru_flagset_init(flagset, flags, flag_count, allocator);
    if (err != YORU_OK)
    {
        trie_destroy(flagset, allocator);
        return (Yoru_FlagParse_Result_t){
            .value = NULL,
            .err = YORU_FLAGPARSE_ERROR_FLAGSET_CREATION_FAILED,
            .message = "Failed to initialize flagset"};
    }

    // skip first argument (program name)
    for (int i = 1; i < argc; i++)
    {
        char *flagname = argv[i];
        res = trie_get_try(flagset, flagname);
        if (res.err != YORU_OK)
        {
            trie_destroy(flagset, allocator);
            return (Yoru_FlagParse_Result_t){
                .err = YORU_FLAGPARSE_ERROR_UNKNOWN_FLAG,
                .message = Yoru_String_to_cstr(Yoru_String_format("Unknown flag '%s'", flagname), allocator)};
        }

        Yoru_Flag_t *flags = (Yoru_Flag_t *)res.value;

        char *flagvalue = argv[i + 1];
        if (!flagvalue && flags->type != YORU_FLAG_TYPE_BOOL)
        {
            trie_destroy(flagset, allocator);
            return (Yoru_FlagParse_Result_t){
                .err = YORU_FLAGPARSE_ERROR_FLAG_REQUIRES_VALUE,
                .message = Yoru_String_to_cstr(Yoru_String_format("Flag '%s' requires a value", flagname), allocator)};
        }
        i++;

        switch (flags->type)
        {
        case YORU_FLAG_TYPE_BOOL:
            if (!flagvalue)
            {
                flags->value.boolean = true;
                flags->is_set = true;
                break;
            }

            if (strcmp(flagvalue, "true") == 0 || strcmp(flagvalue, "1") == 0)
            {
                flags->value.boolean = true;
                flags->is_set = true;
                break;
            }

            if (strcmp(flagvalue, "false") == 0 || strcmp(flagvalue, "0") == 0)
            {
                flags->value.boolean = false;
                flags->is_set = true;
                break;
            }

            trie_destroy(flagset, allocator);
            return (Yoru_FlagParse_Result_t){
                .err = YORU_FLAGPARSE_ERROR_INVALID_VALUE_FOR_FLAG_TYPE,
                .message = Yoru_String_to_cstr(Yoru_String_format("Invalid boolean value for flag '%s'", flagname), allocator)};

        case YORU_FLAG_TYPE_INT:
            char *endptr;
            flags->value.integer = strtoimax(flagvalue, &endptr, 10);

            if (*endptr != '\0')
            {
                trie_destroy(flagset, allocator);
                return (Yoru_FlagParse_Result_t){
                    .err = YORU_FLAGPARSE_ERROR_INVALID_VALUE_FOR_FLAG_TYPE,
                    .message = Yoru_String_to_cstr(Yoru_String_format("Invalid integer value for flag '%s'", flagname), allocator)};
            }
            flags->is_set = true;
            break;

        case YORU_FLAG_TYPE_UINT:
            char *endptr_u;
            flags->value.uinteger = strtoumax(flagvalue, &endptr_u, 10);

            if (*endptr_u != '\0')
            {
                trie_destroy(flagset, allocator);
                return (Yoru_FlagParse_Result_t){
                    .err = YORU_FLAGPARSE_ERROR_INVALID_VALUE_FOR_FLAG_TYPE,
                    .message = Yoru_String_to_cstr(Yoru_String_format("Invalid unsigned integer value for flag '%s'", flagname), allocator)};
            }
            flags->is_set = true;
            break;

        case YORU_FLAG_TYPE_FLOAT:
            char *endptr_f;
            flags->value.floating = strtod(flagvalue, &endptr_f);

            if (*endptr_f != '\0')
            {
                trie_destroy(flagset, allocator);
                return (Yoru_FlagParse_Result_t){
                    .err = YORU_FLAGPARSE_ERROR_INVALID_VALUE_FOR_FLAG_TYPE,
                    .message = Yoru_String_to_cstr(Yoru_String_format("Invalid float value for flag '%s'", flagname), allocator)};
            }
            flags->is_set = true;
            break;

        case YORU_FLAG_TYPE_STRING:
            flags->value.string = flagvalue;
            flags->is_set = true;
            break;

        default:
            trie_destroy(flagset, allocator);
            return (Yoru_FlagParse_Result_t){
                .err = YORU_FLAGPARSE_ERROR_INVALID_VALUE_FOR_FLAG_TYPE,
                .message = Yoru_String_to_cstr(Yoru_String_format("Unknown flag type for flag '%s'", flagname), allocator)};
        }
    }

    for (size_t i = 0; i < flag_count; ++i)
    {
        if (flags[i].is_required && !flags[i].is_set)
        {
            return (Yoru_FlagParse_Result_t){
                .err = YORU_FLAGPARSE_ERROR_REQUIRED_FLAG_NOT_SET,
                .message = Yoru_String_to_cstr(Yoru_String_format("Required flag '%s' is not set", flags[i].name), allocator)};
        }
    }

    trie_destroy(flagset, allocator);
    return (Yoru_FlagParse_Result_t){.value = flags, .err = YORU_FLAGPARSE_ERROR_OK};
}

YORU_API void yoru_print_flag(Yoru_Flag_t *flag)
{
    if (!flag)
    {
        printf("[NULL]\n");
        return;
    }

    printf("Flag[name=%s; short_name=%s; type=%d; required=%d; set=%d; value=",
           flag->name ? flag->name : "<NONE>",
           flag->short_name ? flag->short_name : "<NONE>",
           flag->type,
           flag->is_required,
           flag->is_set);

    switch (flag->type)
    {
    case YORU_FLAG_TYPE_BOOL:
        printf("%s", flag->value.boolean ? "true" : "false");
        break;
    case YORU_FLAG_TYPE_INT:
        printf("%lld", flag->value.integer);
        break;
    case YORU_FLAG_TYPE_UINT:
        printf("%llu", flag->value.uinteger);
        break;
    case YORU_FLAG_TYPE_FLOAT:
        printf("%f", flag->value.floating);
        break;
    case YORU_FLAG_TYPE_STRING:
        printf("%s", flag->value.string ? flag->value.string : "<NONE>");
        break;
    default:
        printf("unknown");
        break;
    }
    printf("]\n");
}

YORU_HELPER Yoru_Error_t yoru_flagset_init(
    TrieNode_t *trie,
    Yoru_Flag_t *flags,
    size_t flag_count,
    Yoru_Allocator_t *allocator)
{
    Yoru_FlagParse_Error_t err;
    for (size_t i = 0; i < flag_count; ++i)
    {
        if (!flags[i].name && !flags[i].short_name)
        {
            return YORU_FLAGPARSE_ERROR_FLAG_HAS_NO_NAME;
        }

        if (flags[i].name)
        {
            err = trie_put_try(trie, flags[i].name, &flags[i], allocator);
            if (err != YORU_OK)
            {
                return err;
            }
        }

        if (flags[i].short_name)
        {
            err = trie_put_try(trie, flags[i].short_name, &flags[i], allocator);
            if (err != YORU_OK)
            {
                return err;
            }
        }
    }

    return YORU_OK;
}

bool yoru_all_required_flags_set(
    Yoru_Flag_t *flags,
    size_t flag_count)
{
    for (size_t i = 0; i < flag_count; ++i)
    {
        if (flags[i].is_required && !flags[i].is_set)
        {
            return false;
        }
    }
    return true;
}

#endif

#endif