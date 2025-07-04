#ifndef __YORU_TESTS_H__
#define __YORU_TESTS_H__

typedef struct
{
    bool success;
    const char *message;
} YoruTestResult_t;

typedef struct
{
    const char *name;
    YoruTestResult_t (*test_func)(void);
} YoruTestFunc_t;

typedef struct
{
    YoruTestFunc_t *tests;
    size_t test_count;
    size_t passed;
    size_t failed;
} YoruTestSuite_t;

u8 YoruTest_run(YoruTestSuite_t *suite)
{
    suite->passed = 0;
    suite->failed = 0;

    for (size_t i = 0; i < suite->test_count; i++)
    {
        YoruTestFunc_t *test = &suite->tests[i];
        YoruTestResult_t result = test->test_func();
        if (result.success)
        {
            suite->passed++;
            printf("✅ '%s' passed.\n", test->name);
            continue;
        }

        suite->failed++;
        printf("❌ '%s' failed: %s\n", test->name, result.message);
    }

    printf("Test suite completed: %zu passed, %zu failed.\n", suite->passed, suite->failed);
    return suite->failed > 0 ? 1 : 0;
}

#endif