/*
    Copyright (C) 2020 Fredrik Johansson

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "fmpz_mod_mpoly_q.h"

int
fmpz_mod_mpoly_q_is_canonical(const fmpz_mod_mpoly_q_t res, const fmpz_mod_mpoly_ctx_t ctx)
{
    if (!fmpz_mod_mpoly_is_canonical(fmpz_mod_mpoly_q_numref(res), ctx))
        return 0;

    if (!fmpz_mod_mpoly_is_canonical(fmpz_mod_mpoly_q_denref(res), ctx))
        return 0;

    if (fmpz_mod_mpoly_is_zero(fmpz_mod_mpoly_q_denref(res), ctx))
        return 0;

    if (fmpz_sgn(fmpz_mod_mpoly_q_denref(res)->coeffs) < 0)
        return 0;

    {
        int ans;
        int ans_coeff = 0;
        fmpz_t g_coeff;
        fmpz_mod_mpoly_t g;
        fmpz_init(g_coeff);
        fmpz_mod_mpoly_init(g, ctx);

        fmpz_mod_mpoly_gcd_assert_successful(g, fmpz_mod_mpoly_q_numref(res), fmpz_mod_mpoly_q_denref(res), ctx);

        ans = fmpz_mod_mpoly_is_one(g, ctx);
        fmpz_gcd(g_coeff, fmpz_mod_mpoly_q_numref(res)->coeffs, fmpz_mod_mpoly_q_denref(res)->coeffs);
        
        ans_coeff = fmpz_mod_mpoly_is_one(g_coeff, ctx);
        fmpz_mod_mpoly_clear(g, ctx);

        return ans*ans_coeff;
    }
}
