/*
    Copyright (C) 2015 William Hart
    Copyright (C) 2015 Kushagra Singh

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "test_helpers.h"
#include "gmpcompat.h"
#include "ulong_extras.h"

TEST_FUNCTION_START(n_cbrt_chebyshev_approx, state)
{
    int i, result;

    /* random n */

    for (i = 0; i < 1000 * flint_test_multiplier(); i++)
    {
        ulong n, val, ans;
        mpz_t mpz_n, mpz_val;

        mpz_init(mpz_n);
        mpz_init(mpz_val);

        n = n_randtest_not_zero(state);
        val = n_cbrt_chebyshev_approx(n);

        flint_mpz_set_ui(mpz_n, n);
        mpz_root(mpz_val, mpz_n, 3);
        ans = flint_mpz_get_ui(mpz_val);

        result = (val == ans);
        if (!result)
            TEST_FUNCTION_FAIL("n = %wu, val = %wd, ans = %wu\n", n, val, ans);

        mpz_clear(mpz_n);
        mpz_clear(mpz_val);
    }
    /* type n^3 + k */

    for (i = 0; i < 1000 * flint_test_multiplier(); i++)
    {
        ulong n, val, ans, bits;
        mpz_t mpz_n, mpz_val;

        mpz_init(mpz_n);
        mpz_init(mpz_val);

        do {
            bits = n_randint(state, FLINT_BITS/3 + 1);
            n = n_randtest_bits(state, bits);
            n = n*n*n;
            n += (n_randint(state, 100) - 50);
        } while (n == 0);
        val = n_cbrt_chebyshev_approx(n);

        flint_mpz_set_ui(mpz_n, n);
        mpz_root(mpz_val, mpz_n, 3);
        ans = flint_mpz_get_ui(mpz_val);

        result = (val == ans);
        if (!result)
            TEST_FUNCTION_FAIL("n = %wu, val = %wd, ans = %wu\n", n, val, ans);

        mpz_clear(mpz_n);
        mpz_clear(mpz_val);
    }

    /* type n^3 + 1 */

    for (i = 0; i < 1000 * flint_test_multiplier(); i++)
    {
        ulong n, val, ans, bits;
        mpz_t mpz_n, mpz_val;

        mpz_init(mpz_n);
        mpz_init(mpz_val);

        do {
            bits = n_randint(state, FLINT_BITS/3 + 1);
            n = n_randtest_bits(state, bits);
            n = n*n*n;
            n += 1;
        } while (n == 0);
        val = n_cbrt_chebyshev_approx(n);

        flint_mpz_set_ui(mpz_n, n);
        mpz_root(mpz_val, mpz_n, 3);
        ans = flint_mpz_get_ui(mpz_val);

        result = (val == ans);
        if (!result)
            TEST_FUNCTION_FAIL("n = %wu, val = %wd, ans = %wu\n", n, val, ans);

        mpz_clear(mpz_n);
        mpz_clear(mpz_val);
    }

    /* type n^3 - 1 */

    for (i = 0; i < 1000 * flint_test_multiplier(); i++)
    {
        ulong n, val, ans, bits;
        mpz_t mpz_n, mpz_val;

        mpz_init(mpz_n);
        mpz_init(mpz_val);

        do {
            bits = n_randint(state, FLINT_BITS/3 + 1);
            n = n_randtest_bits(state, bits);
            n = n*n*n;
            n -= 1;
        } while (n == 0);
        val = n_cbrt_chebyshev_approx(n);

        flint_mpz_set_ui(mpz_n, n);
        mpz_root(mpz_val, mpz_n, 3);
        ans = flint_mpz_get_ui(mpz_val);

        result = (val == ans);
        if (!result)
            TEST_FUNCTION_FAIL("n = %wu, val = %wd, ans = %wu\n", n, val, ans);

        mpz_clear(mpz_n);
        mpz_clear(mpz_val);
    }

    TEST_FUNCTION_END(state);
}
