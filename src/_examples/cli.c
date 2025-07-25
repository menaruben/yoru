#define YORU_IMPLEMENTATION
#include "../yoru.h"
#include <stdbool.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    Yoru_Flag_t flags[] = {
        yoru_flag_new("-i", "--int", "An integer flag", true, YORU_FLAG_TYPE_INT),
        yoru_flag_new("-f", "--float", "A float flag", false, YORU_FLAG_TYPE_FLOAT),
        yoru_flag_new("-s", "--string", "A string flag", false, YORU_FLAG_TYPE_STRING),
        yoru_flag_new("-b", "--bool", "A boolean flag", false, YORU_FLAG_TYPE_BOOL),
        yoru_flag_new("-u", "--uint", "An unsigned integer flag", false, YORU_FLAG_TYPE_UINT)};

    size_t flag_count = sizeof(flags) / sizeof(Yoru_Flag_t);

    Yoru_FlagParse_Error_t err = yoru_flagset_parse(argc, argv, flags, flag_count);
    if (err.err != YORU_FLAGPARSE_ERROR_OK)
    {
        const char *err_msg = yoru_flagparse_error_to_string(err.err);
        printf("Error parsing flags: %s\n", err_msg);
        yoru_print_help(flags, flag_count, "yoru_cli_example", "This is an example of a command line interface using Yoru flags.");
        return 1;
    }

    printf("Flags parsed successfully:\n");
    for (size_t i = 0; i < flag_count; ++i)
    {
        if (flags[i].is_set)
        {
            yoru_print_flag(&flags[i]);
        }
    }

    return 0;
}