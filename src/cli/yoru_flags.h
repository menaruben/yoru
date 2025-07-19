#ifndef __YORU_FLAGS_H__
#define __YORU_FLAGS_H__

#include <stdbool.h>
#include <stddef.h>
#include "../inttypes/yoru_inttypes.h"
#include "../memory/yoru_memory.h"
#include "../yoru_defs.h"
#include "../results/yoru_results.h"
#include "string.h"
#include "../utils/yoru_utils.h"
#include "../collections/yoru_trie.h"

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
    ((Yoru_Flag_t){short_name, name, description, is_required, false, {0}, type})

typedef enum
{
    YORU_FLAGPARSE_ERROR_OK,
    YORU_FLAGPARSE_ERROR_REQUIRED_FLAG_NOT_SET,
    YORU_FLAGPARSE_ERROR_UNKNOWN_FLAG,
    YORU_FLAGPARSE_ERROR_INVALID_VALUE_FOR_FLAG_TYPE,
    YORU_FLAGPARSE_ERROR_FLAGSET_CREATION_FAILED,
    YORU_FLAGPARSE_ERROR_FLAG_HAS_NO_NAME,
    YORU_FLAGPARSE_ERROR_FLAG_REQUIRES_VALUE,
} Yoru_FlagParse_ErrorType_t;

typedef struct
{
    Yoru_FlagParse_ErrorType_t err;
    const char *message;
} Yoru_FlagParse_Error_t;

YORU_API const char *yoru_flagparse_error_to_string(Yoru_FlagParse_ErrorType_t err);

YORU_API void yoru_print_help(
    Yoru_Flag_t *flags,
    size_t flag_count,
    const char *program_name,
    const char *description);

YORU_API Yoru_FlagParse_Error_t yoru_flagset_parse(
    int argc,
    char **argv,
    Yoru_Flag_t *flags,
    size_t flag_count);

YORU_API void yoru_print_flag(Yoru_Flag_t *flag);

YORU_HELPER Yoru_Error_t yoru_flagset_init(
    Yoru_TrieNode_t *trie,
    Yoru_Flag_t *flags,
    size_t flag_count);

YORU_HELPER bool yoru_all_required_flags_set(
    Yoru_Flag_t *flags,
    size_t flag_count);

#ifdef YORU_IMPLEMENTATION

YORU_API const char *yoru_flagparse_error_to_string(Yoru_FlagParse_ErrorType_t err)
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

YORU_API Yoru_FlagParse_Error_t yoru_flagset_parse(
    int argc,
    char **argv,
    Yoru_Flag_t *flags,
    size_t flag_count)
{
    Yoru_Error_t err;
    Yoru_TrieNode_t flagset = {0};
    err = yoru_trie_init(&flagset, sizeof(Yoru_Flag_t));
    if (err.type != YORU_OK)
    {
        return (Yoru_FlagParse_Error_t){
            .err = YORU_FLAGPARSE_ERROR_FLAGSET_CREATION_FAILED,
            .message = "Failed to initialize flagset"};
    }

    err = yoru_flagset_init(&flagset, flags, flag_count);
    if (err.type != YORU_OK)
    {
        yoru_trie_free(&flagset);
        return (Yoru_FlagParse_Error_t){
            .err = YORU_FLAGPARSE_ERROR_FLAGSET_CREATION_FAILED,
            .message = "Failed to initialize flagset with provided flags"};
    }

    // skip first argument (program name)
    for (int i = 1; i < argc; i++)
    {
        char *flagname = argv[i];
        Yoru_Flag_t flag = {0};
        err = yoru_trie_get(&flagset, flagname, &flag);
        if (err.type != YORU_OK)
        {
            yoru_trie_free(&flagset);
            return (Yoru_FlagParse_Error_t){
                .err = YORU_FLAGPARSE_ERROR_UNKNOWN_FLAG,
                .message = YORU_TMPSTR("Unknown flag '", flagname, "'")};
        }

        char *flagvalue = argv[i + 1];
        if (!flagvalue && flag.type != YORU_FLAG_TYPE_BOOL)
        {
            yoru_trie_free(&flagset);
            return (Yoru_FlagParse_Error_t){
                .err = YORU_FLAGPARSE_ERROR_FLAG_REQUIRES_VALUE,
                .message = YORU_TMPSTR("Flag '", flagname, "' requires a value")};
        }
        i++;

        switch (flag.type)
        {
        case YORU_FLAG_TYPE_BOOL:
            if (!flagvalue)
            {
                flag.value.boolean = true;
                flag.is_set = true;
                break;
            }

            if (strcmp(flagvalue, "true") == 0 || strcmp(flagvalue, "1") == 0)
            {
                flag.value.boolean = true;
                flag.is_set = true;
                break;
            }

            if (strcmp(flagvalue, "false") == 0 || strcmp(flagvalue, "0") == 0)
            {
                flag.value.boolean = false;
                flag.is_set = true;
                break;
            }

            yoru_trie_free(&flagset);
            return (Yoru_FlagParse_Error_t){
                .err = YORU_FLAGPARSE_ERROR_INVALID_VALUE_FOR_FLAG_TYPE,
                .message = YORU_TMPSTR("Invalid boolean value for flag '", flagname, "'")};

        case YORU_FLAG_TYPE_INT:
            char *endptr;
            flag.value.integer = strtoimax(flagvalue, &endptr, 10);

            if (*endptr != '\0')
            {
                yoru_trie_free(&flagset);
                return (Yoru_FlagParse_Error_t){
                    .err = YORU_FLAGPARSE_ERROR_INVALID_VALUE_FOR_FLAG_TYPE,
                    .message = YORU_TMPSTR("Invalid integer value for flag '", flagname, "'")};
            }
            flag.is_set = true;
            break;

        case YORU_FLAG_TYPE_UINT:
            char *endptr_u;
            flag.value.uinteger = strtoumax(flagvalue, &endptr_u, 10);

            if (*endptr_u != '\0')
            {
                yoru_trie_free(&flagset);
                return (Yoru_FlagParse_Error_t){
                    .err = YORU_FLAGPARSE_ERROR_INVALID_VALUE_FOR_FLAG_TYPE,
                    .message = YORU_TMPSTR("Invalid unsigned integer value for flag '", flagname, "'")};
            }
            flag.is_set = true;
            break;

        case YORU_FLAG_TYPE_FLOAT:
            char *endptr_f;
            flag.value.floating = strtod(flagvalue, &endptr_f);

            if (*endptr_f != '\0')
            {
                yoru_trie_free(&flagset);
                return (Yoru_FlagParse_Error_t){
                    .err = YORU_FLAGPARSE_ERROR_INVALID_VALUE_FOR_FLAG_TYPE,
                    .message = YORU_TMPSTR("Invalid float value for flag '", flagname, "'")};
            }
            flag.is_set = true;
            break;

        case YORU_FLAG_TYPE_STRING:
            flag.value.string = flagvalue;
            flag.is_set = true;
            break;

        default:
            yoru_trie_free(&flagset);
            return (Yoru_FlagParse_Error_t){
                .err = YORU_FLAGPARSE_ERROR_INVALID_VALUE_FOR_FLAG_TYPE,
                .message = YORU_TMPSTR("Unknown flag type for flag '", flagname, "'")};
        }

        // update the flag in the flagset
        err = yoru_trie_set(&flagset, flagname, &flag);
        if (err.type != YORU_OK)
        {
            yoru_trie_free(&flagset);
            return (Yoru_FlagParse_Error_t){
                .err = YORU_FLAGPARSE_ERROR_FLAGSET_CREATION_FAILED,
                .message = "Failed to set flag value in flagset"};
        }

        for (size_t j = 0; j < flag_count; ++j)
        {
            if (flags[j].name && strcmp(flags[j].name, flagname) == 0)
            {
                flags[j] = flag;
                break;
            }
            if (flags[j].short_name && strcmp(flags[j].short_name, flagname) == 0)
            {
                flags[j] = flag;
                break;
            }
        }
    }

    for (size_t i = 0; i < flag_count; ++i)
    {
        if (flags[i].is_required && !flags[i].is_set)
        {
            return (Yoru_FlagParse_Error_t){
                .err = YORU_FLAGPARSE_ERROR_REQUIRED_FLAG_NOT_SET,
                .message = YORU_TMPSTR("Required flag '", flags[i].name ? flags[i].name : flags[i].short_name, "' is not set")};
        }
    }

    yoru_trie_free(&flagset);
    return (Yoru_FlagParse_Error_t){.err = YORU_FLAGPARSE_ERROR_OK, .message = NULL};
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
    Yoru_TrieNode_t *trie,
    Yoru_Flag_t *flags,
    size_t flag_count)
{
    Yoru_FlagParse_Error_t flag_err;
    Yoru_Error_t yoru_err;
    for (size_t i = 0; i < flag_count; ++i)
    {
        if (!flags[i].name && !flags[i].short_name)
        {
            return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_INVALID, .message = "Flag must have at least a name or short name"};
        }

        if (flags[i].name)
        {
            yoru_err = yoru_trie_set(trie, flags[i].name, &flags[i]);
            if (yoru_err.type != YORU_OK)
            {
                return yoru_err;
            }
        }

        if (flags[i].short_name)
        {
            yoru_err = yoru_trie_set(trie, flags[i].short_name, &flags[i]);
            if (yoru_err.type != YORU_OK)
            {
                return yoru_err;
            }
        }
    }

    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
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
