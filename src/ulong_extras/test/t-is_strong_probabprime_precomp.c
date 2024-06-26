/*
    Copyright (C) 2009 William Hart

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "test_helpers.h"
#include "gmpcompat.h"
#include "ulong_extras.h"

TEST_FUNCTION_START(n_is_strong_probabprime_precomp, state)
{
    int i, j, result;
    ulong count = UWORD(0);
    slong test_multiplier;

    test_multiplier = FLINT_MAX(1, flint_test_multiplier());

    for (i = 0; i < 100 * test_multiplier; i++) /* Test that primes pass the test */
    {
        ulong a, d, norm;
        mpz_t d_m;
        double dpre;
        ulong bits = n_randint(state, FLINT_D_BITS-1) + 2;

        mpz_init(d_m);

        do
        {
            d = n_randbits(state, bits) | 1;
            flint_mpz_set_ui(d_m, d);
            mpz_nextprime(d_m, d_m);
            d = flint_mpz_get_ui(d_m);
        } while (FLINT_BIT_COUNT(d) > FLINT_D_BITS);
        if (d == UWORD(2)) d++;

        for (j = 0; j < 100; j++)
        {
            do a = n_randint(state, d);
            while (a == UWORD(0));

            dpre = n_precompute_inverse(d);
            norm = flint_ctz(d - 1);
            result = n_is_strong_probabprime_precomp(d, dpre, a, (d - 1)>>norm);

            if (!result)
                TEST_FUNCTION_FAIL("a = %wu, d = %wu\n", a, d);
        }

        mpz_clear(d_m);
    }

    for (i = 0; i < 100 * test_multiplier; i++) /* Test that not too many composites pass */
    {
        ulong a, d, norm;
        mpz_t d_m;
        double dpre;
        ulong bits = n_randint(state, FLINT_D_BITS-3) + 4;

        mpz_init(d_m);

        do
        {
            d = n_randbits(state, bits) | 1;
            flint_mpz_set_ui(d_m, d);
        } while (mpz_probab_prime_p(d_m, 12));

        for (j = 0; j < 100; j++)
        {
            do a = n_randint(state, d);
            while (a == UWORD(0));

            dpre = n_precompute_inverse(d);
            norm = flint_ctz(d - 1);
            result = !n_is_strong_probabprime_precomp(d, dpre, a, (d - 1)>>norm);

            if (!result) count++;
        }

        mpz_clear(d_m);
    }

    if (count > 220 * test_multiplier)
        TEST_FUNCTION_FAIL("count = %wu\n", count);

    TEST_FUNCTION_END(state);
}
