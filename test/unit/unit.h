// Copyright 2016 Douglas Moore. All rights reserved.
// Use of this source code is governed by the MIT
// license that can be found in the LICENSE file.
#pragma once

#include <assert.h>
#include <float.h>
#include <inttypes.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct unit
{
    char const *name;
    void (*run)(void);
};

struct unit_suite
{
    char const *name;
    int total;
    int num_ok;
    int num_failed;
    struct unit *units[];
};

extern jmp_buf __unit_err;

inline static int run_unit(struct unit_suite *suite, struct unit *u)
{
    if (u == NULL) return 1;

    printf("  [TEST] %s ", u->name);

    assert(u->run != NULL);

    ++suite->total;

    int result = setjmp(__unit_err);
    if (result == 0)
    {
        u->run();
        printf(" [OK]\n");
        ++suite->num_ok;
    }
    else
    {
        printf(" [FAIL]\n");
        ++suite->num_failed;
    }
    return 0;
}

inline static void run_unit_suite(struct unit_suite *suite)
{
    if (suite == NULL) return;

    printf("[SUITE] %s\n", suite->name);

    struct unit **u = suite->units;
    while (*u != NULL)
    {
        if (run_unit(suite, *u) != 0)
        {
            printf("[FRAMEWORK FAILURE]\n");
            abort();
        }
        ++u;
    }
}

#define __FUNCNAME(NAME) __unit_test_##NAME##_body
#define __UNITNAME(NAME) __unit_test_##NAME
#define UNIT(NAME) \
    static void __FUNCNAME(NAME)(void); \
    static struct unit __UNITNAME(NAME) = { \
        .name = #NAME, \
        .run  = __FUNCNAME(NAME) \
    }; \
    static void __FUNCNAME(NAME)(void)

#define __SUITENAME(NAME) __unit_suite_##NAME
#define BEGIN_SUITE(NAME) \
    struct unit_suite __SUITENAME(NAME) = { \
        .name = #NAME, \
        .total = 0, \
        .num_ok = 0, \
        .num_failed = 0, \
        .units = {
#define ADD_UNIT(NAME) &__UNITNAME(NAME),
#define END_SUITE NULL }};

#define IMPORT_SUITE(NAME) extern struct unit_suite __SUITENAME(NAME)
#define BEGIN_REGISTRATION \
    jmp_buf __unit_err; \
    static struct unit_suite *__unit_suites[] = {
#define REGISTER(NAME) &__SUITENAME(NAME),
#define END_REGISTRATION NULL };

#define UNIT_MAIN() \
    int main() \
    { \
        srand((unsigned int) time(NULL)); \
        int total = 0, num_ok = 0, num_failed = 0; \
        size_t n = sizeof(__unit_suites) / sizeof(struct unit_suite *); \
        size_t i; \
        for (i = 0; i < n; ++i) \
        { \
            if (__unit_suites[i] == NULL) break; \
            run_unit_suite(__unit_suites[i]); \
            total += __unit_suites[i]->total; \
            num_ok += __unit_suites[i]->num_ok; \
            num_failed += __unit_suites[i]->num_failed; \
        } \
        printf("RESULTS: %d tests (%d ok, %d failed)\n", total, num_ok, num_failed); \
        return num_failed; \
    }

inline static void unit_error(char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    longjmp(__unit_err, 1);
}

inline static void assert_equal(intmax_t exp, intmax_t real, char const *caller, int line);
#define ASSERT_EQUAL(exp, real) assert_equal(exp, real, __FILE__, __LINE__)

inline static void assert_equal_u(uintmax_t exp, uintmax_t real, char const *caller, int line);
#define ASSERT_EQUAL_U(exp, real) assert_equal_u(exp, real, __FILE__, __LINE__)

inline static void assert_true(int real, char const *caller, int line);
#define ASSERT_TRUE(real) assert_true(real, __FILE__, __LINE__)

inline static void assert_false(int real, char const *caller, int line);
#define ASSERT_FALSE(real) assert_false(real, __FILE__, __LINE__)

inline static void assert_null(void *real, char const *caller, int line);
#define ASSERT_NULL(real) assert_null(real, __FILE__, __LINE__)

inline static void assert_not_null(void *real, char const *caller, int line);
#define ASSERT_NOT_NULL(real) assert_not_null(real, __FILE__, __LINE__)

inline static void assert_dbl_near(double exp, double real, double tol, char const *caller, int line);
#define ASSERT_DBL_NEAR_TOL(exp, real, tol) assert_dbl_near(exp, real, tol, __FILE__, __LINE__)
#define ASSERT_DBL_NEAR(exp, real) assert_dbl_near(exp, real, DBL_EPSILON, __FILE__, __LINE__)

inline static void assert_equal(intmax_t exp, intmax_t real, char const *caller, int line)
{
    if (exp != real)
    {
        unit_error("%s:%d - expected %" PRIdMAX ", got %" PRIdMAX, caller, line, exp, real);
    }
}

inline static void assert_equal_u(uintmax_t exp, uintmax_t real, char const *caller, int line)
{
    if (exp != real)
    {
        unit_error("%s:%d - expected %" PRIdMAX ", got %" PRIdMAX, caller, line, exp, real);
    }
}

inline static void assert_true(int real, char const *caller, int line)
{
    if (real == 0)
    {
        unit_error("%s:%d should be true", caller, line);
    }
}

inline static void assert_false(int real, char const *caller, int line)
{
    if (real != 0)
    {
        unit_error("%s:%d should be false", caller, line);
    }
}

inline static void assert_null(void *real, const char *caller, int line)
{
    if (real != NULL)
    {
        unit_error("%s:%d should be NULL", caller, line);
    }
}

inline static void assert_not_null(void *real, const char *caller, int line)
{
    if (real == NULL)
    {
        unit_error("%s:%d should not be NULL", caller, line);
    }
}

inline static void assert_dbl_near(double exp, double real, double tol, char const *caller, int line)
{
    double diff = exp - real;
    tol += DBL_EPSILON;
    double absdiff = (diff < 0.) ? -diff : diff;
    if (absdiff > tol)
    {
        unit_error("%s:%d expected %0.3e, got %0.3e (diff %0.3e, tol %0.3e)", caller, line, exp, real, diff, tol);
    }
}
