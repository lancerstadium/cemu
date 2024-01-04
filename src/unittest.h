/**
 * @file miniunit.h
 * @author lancer (lancerstadium@163.com)
 * @brief 单元测试头文件
 * @version 0.1
 * @date 2023-12-28
 *
 * @copyright Copyright (c) 2023
 * @note 参考项目：[Gitee | MiniUnit](https://gitee.com/zhuangbo/MiniUnit)
 *
 * A small unit test framework for C/C++.
 *
 * Origin author: Bo Zhuang <sdzhuangbo@hotmail.com>
 * Features:
 *   - `ut_assert(expr)` assertion fail if `expr` false
 *   - `ut_assert(expr, message)` assertion with message
 *   - `ut_assert(expr, message, args...)` assertion with message and args
 *   - `ut_run_test(test)` to run a test function in form `int f()`
 *                         return 0 if passed
 *   - `ut_test_results()` to display the test results
 *   - `#define MU_NOCOLOR` if ANSI escape code not supported
 *
 * For example,
 * ```c
 * #include "miniunit.h"
 *
 * int test_one() {
 *   ut_assert(2 + 2 == 4);
 *   return 0; // 0 means test passed
 * }
 *
 * int test_two() {
 *   int a = 3, b = 5;
 *   ut_assert(a == 3);
 *   ut_assert(b == 5, "b is 5");
 *   ut_assert(a + b == 7, "should be %d", a + b); // fail
 *   return 0;
 * }
 *
 * int main()
 * {
 *   ut_run_test(test_one);
 *   ut_run_test(test_two);
 *
 *   ut_test_results();
 *
 *   return 0;
 * }
 * ```
 */

#ifndef UNIT_TEST_H
#define UNIT_TEST_H

#include <stdio.h>
#include "color.h"

/* Count of args */
#define VA_NARG(args...) VA_NARG_(0, ##args, VA_RSEQ_N())
#define VA_NARG_(args...) VA_ARG_N(args)
#define VA_ARG_N(_0, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, N, ...) N
#define VA_RSEQ_N() 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

/* The first arg */
#define VA_FIRST(args...) VA_FIRST_(args)
#define VA_FIRST_(F, ...) F

/* The first arg in string */
#define VA_FIRST_STR(args...) VA_FIRST_STR_(args)
#define VA_FIRST_STR_(F, ...) #F

/* The rest args */
#define VA_REST(args...) VA_REST_(args)
#define VA_REST_(F, args...) args

/* Define MU_NOCOLOR if not linux */
#ifndef __linux__
#ifndef MU_NOCOLOR
#define MU_NOCOLOR
#endif
#endif /* not linux */

/* ANSI escape code */
#ifdef MU_NOCOLOR

#define NULSTR ""

#define RST NULSTR
#define BOLD NULSTR
#define UNDL NULSTR

/* foreground */
#define FBLK NULSTR
#define FRED NULSTR
#define FGRN NULSTR
#define FYEL NULSTR
#define FBLU NULSTR
#define FMAG NULSTR
#define FCYN NULSTR
#define FWHT NULSTR

#else /* MU_NOCOLOR */

#define RST "\x1B[0m"
#define BOLD "\x1B[1m"
#define UNDL "\x1B[4m"

/* foreground */
#define FBLK "\x1B[30m"
#define FRED "\x1B[31m"
#define FGRN "\x1B[32m"
#define FYEL "\x1B[33m"
#define FBLU "\x1B[34m"
#define FMAG "\x1B[35m"
#define FCYN "\x1B[36m"
#define FWHT "\x1B[37m"

#endif /* MU_NOCOLOR */

#define ENDL RST "\n"
#define BORED BOLD FRED
#define BOGRN BOLD FGRN
#define BOYEL BOLD FYEL
#define BOWHT BOLD FWHT

#ifdef __linux__
#define FAIL "✘"
#define PASS "✔"
#else /* for others */
#define FAIL "FAIL"
#define PASS "PASS"
#endif /* __linux__ */

struct unittest_t
{
    int n_test; /* number of tests */
    int n_pass; /* number of tests passed */
    int n_fail; /* number of tests failed */
    int u_t;
    int u_p;
    int u_f;
    int flag;
    int quiet;
};

static struct unittest_t ut_t = {
    .n_fail = 0,
    .n_pass = 0,
    .n_test = 0,
    .u_f = 0,
    .u_p = 0,
    .u_t = 0,
    .flag = 0,
    .quiet = 0};

#define ut_set_quiet(num) \
    ut_t.quiet = num;

#define ut_assert(test...)                                                                            \
    do                                                                                                \
    {                                                                                                 \
        ut_t.u_t++;                                                                                   \
        if (!(VA_FIRST(test)))                                                                        \
        {                                                                                             \
            ut_t.u_f++;                                                                               \
            ut_t.flag = 1;                                                                            \
            printf(ENDL "|--- " _bred("FAIL %d") " " _black("%s:%d") ": '" BOYEL "%s" RST "' ",       \
                   ut_t.u_t,                                                                          \
                   __FILE__, __LINE__, VA_FIRST_STR(test));                                           \
            if (VA_NARG(test) == 1 || ut_t.quiet > 1)                                                 \
                printf(_bred(FAIL));                                                                  \
            else                                                                                      \
                printf(_bred(FAIL) ENDL "|-----< " FMAG VA_REST(test));                               \
            break;                                                                                    \
        }                                                                                             \
        else                                                                                          \
        {                                                                                             \
            ut_t.u_p++;                                                                               \
            if (ut_t.quiet <= 0)                                                                      \
            {                                                                                         \
                printf(ENDL "|--- " _bgreen("PASS %d") " " _black("%s:%d") ": '" BOYEL "%s" RST "' ", \
                       ut_t.u_t,                                                                      \
                       __FILE__, __LINE__, VA_FIRST_STR(test));                                       \
                if (VA_NARG(test) == 1 || ut_t.quiet > 1)                                             \
                    printf(_bgreen(PASS));                                                            \
                else                                                                                  \
                    printf(_bgreen(PASS) ENDL "|-----< " FMAG VA_REST(test));                         \
            }                                                                                         \
            break;                                                                                    \
        }                                                                                             \
    } while (0)

#define ut_dec_test(testname) \
    int testname##_test()

#define ut_def_test(testname, ...)                                                          \
    int testname##_test()                                                                   \
    {                                                                                       \
        ut_t.flag = 0;                                                                      \
        ut_t.u_t = 0, ut_t.u_p = 0, ut_t.u_f = 0;                                           \
        __VA_ARGS__                                                                         \
        if (ut_t.quiet < 3)                                                                 \
        {                                                                                   \
            printf(ENDL "|--- Test %s" BOGRN " %d " PASS RST " and ", #testname, ut_t.u_p); \
            printf(ut_t.n_fail ? BORED "%d " FAIL : BOLD FBLK "%d " FAIL, ut_t.u_f);        \
            printf(RST " in " _bold("%d") " asserts.", ut_t.u_t);                           \
        }                                                                                   \
        return ut_t.flag;                                                                   \
    }

#define ut_run_test(testname)                                          \
    do                                                                 \
    {                                                                  \
        if (ut_t.n_test == 0)                                          \
        {                                                              \
            printf(ENDL "|============== Unit tests ==============="); \
        }                                                              \
        ++ut_t.n_test;                                                 \
        printf(ENDL "|- " _ubold("%s") _black(" %s:%d ... "),          \
               #testname, __FILE__, __LINE__);                         \
        if (!testname##_test())                                        \
        {                                                              \
            ++ut_t.n_pass;                                             \
            if (ut_t.quiet > 2)                                        \
            {                                                          \
                printf(_bgreen(PASS));                                 \
            }                                                          \
        }                                                              \
        else                                                           \
        {                                                              \
            ++ut_t.n_fail;                                             \
        }                                                              \
    } while (0)

#define ut_print_test()                                                                               \
    do                                                                                                \
    {                                                                                                 \
        printf(ENDL "|============== Test Result ==============");                                    \
        printf(ENDL "| " BOGRN "%d " PASS RST " and ", ut_t.n_pass);                                  \
        printf(ut_t.n_fail ? BORED "%d " FAIL : BOLD FBLK "%d " FAIL, ut_t.n_fail);                   \
        printf(RST " in " BOWHT "%d" RST " TEST(S)", ut_t.n_test);                                    \
        if (ut_t.n_pass == ut_t.n_test)                                                               \
            printf(ENDL "|============" _bgreen(" ALL TESTS PASSED ") "===========");                 \
        else                                                                                          \
            printf(ENDL "|===========" _red(" %d TEST(S) FAILED ") "============" ENDL, ut_t.n_fail); \
    } while (0)

#endif /* UNIT_TEST_H */
