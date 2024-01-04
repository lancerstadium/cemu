#include "utils.h"


ut_def_test(one,
    int a = 3, b = 5;
    ut_assert(a == 3);
    ut_assert(b == 5);
)

ut_def_test(two,
    int a = 3, b = 5;
    ut_assert(a == 3);
    ut_assert(b == 4, "b is 5");
)

ut_def_test(byte_and_int,
    int x1 = INT32_MIN;
    ut_assert((x1*2) < 0, "case1: x1 < 0 => x1 * 2 <0");

    unsigned ux = 5;
    ut_assert(ux > -1, "case2: unsigned %d < -1", ux);

    int x2 = 1;
    int y = INT32_MIN;
    ut_assert(x2 > y, "case3: x2 > y");
    ut_assert(-x2 < -y, "case3: %d < %d", -x2, -y);

    int x3 = 50000;
    ut_assert(x3*x3 == 2500000000, "case4: x3*x3 != x^2");
)

ut_def_test(float_suit,
    ut_assert(+0. == -0., "IEEE 754 stand");
    ut_assert(1./+0. == 1./-0., "+INF != -INF");
)


ut_def_test(big_endian,
    ut_assert(+0. == -0., "IEEE 754 stand");
    ut_assert(1./+0. == 1./-0., "+INF != -INF");
)