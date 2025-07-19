#ifndef __YORU_TESTS_H__
#define __YORU_TESTS_H__

#include <stdbool.h>

typedef struct
{
    bool success;
    const char *message;
} Yoru_TestResult_t;

typedef struct
{
    const char *name;
    Yoru_TestResult_t (*test_func)(void);
} Yoru_TestFunc_t;

typedef struct
{
    Yoru_TestFunc_t *tests;
    size_t test_count;
    size_t passed;
    size_t failed;
} YoruTestSuite_t;

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

u8 Yoru_Test_run(YoruTestSuite_t *suite)
{

#if defined(_WIN32) || defined(_WIN64)
    SetConsoleOutputCP(CP_UTF8);
#endif

    suite->passed = 0;
    suite->failed = 0;

    for (size_t i = 0; i < suite->test_count; i++)
    {
        Yoru_TestFunc_t *test = &suite->tests[i];
        Yoru_TestResult_t result = test->test_func();
        if (result.success)
        {
            suite->passed++;
            printf("✅ '%s' passed.\n", test->name);
            continue;
        }

        suite->failed++;
        printf("❌ '%s' failed: %s\n", test->name, result.message);
    }
    f32 success_ratio = (suite->passed / suite->test_count) * 100;
    printf("Test suite completed: %zu passed, %zu failed, success ratio: %.2f%%\n", suite->passed, suite->failed, success_ratio);
    return suite->failed > 0 ? 1 : 0;
}

#endif