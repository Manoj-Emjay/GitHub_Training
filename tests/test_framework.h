#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>

/* Minimal, dependency-free assertion/runner macros for embedded-style unit tests */

#define TEST_ASSERT(cond) \
    do { \
        if (!(cond)) { \
            printf("\n    assertion failed: %s (line %d)", #cond, __LINE__); \
            return 0; \
        } \
    } while (0)

#define TEST_ASSERT_EQUAL(expected, actual) \
    do { \
        long expected_ = (long)(expected); \
        long actual_ = (long)(actual); \
        if (expected_ != actual_) { \
            printf("\n    expected %ld but got %ld (line %d)", expected_, actual_, __LINE__); \
            return 0; \
        } \
    } while (0)

#define RUN_TEST(fn, totalPtr, passedPtr) \
    do { \
        printf("[RUN ] %-48s ", #fn); \
        (*(totalPtr))++; \
        if (fn()) { \
            printf("[PASS]\n"); \
            (*(passedPtr))++; \
        } else { \
            printf("\n[FAIL] %s\n", #fn); \
        } \
    } while (0)

#endif /* TEST_FRAMEWORK_H */
