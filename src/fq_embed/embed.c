/*
    Copyright (C) 2017 Luca De Feo

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "fmpz_mod_poly.h"
#include "fq.h"
#include "fq_poly.h"
#include "fq_poly_factor.h"
#include "fq_embed.h"

void fq_embed_gens(fq_t gen_sub, fq_t gen_sup, fmpz_mod_poly_t minpoly,
                             const fq_ctx_t sub_ctx,
                             const fq_ctx_t sup_ctx)
{
    if (fq_ctx_degree(sub_ctx) == 1)
    {
        fq_gen(gen_sub, sub_ctx);
        fq_set(gen_sup, gen_sub, sup_ctx);
    }
    else
    {
        _fq_embed_gens_naive(gen_sub, gen_sup, minpoly, sub_ctx, sup_ctx);
    }
}

void _fq_embed_gens_naive(fq_t gen_sub,
                                    fq_t gen_sup,
                                    fmpz_mod_poly_t minpoly,
                                    const fq_ctx_t sub_ctx,
                                    const fq_ctx_t sup_ctx)
{
    fq_poly_t modulus, fact;
    flint_rand_t state;

    fq_poly_init(modulus, sup_ctx);
    fq_poly_init(fact, sup_ctx);
    fq_poly_set_fmpz_mod_poly(modulus, fq_ctx_modulus(sub_ctx), sup_ctx);

    flint_rand_init(state);

    /* Get one linear factor of sub_ctx->modulus in sup_ctx */
    while (fq_poly_degree(modulus, sup_ctx) != 1)
    {
        while (!fq_poly_factor_equal_deg_prob(fact, state, modulus, 1, sup_ctx))
        {
        };
        fq_poly_set(modulus, fact, sup_ctx);
    }

    flint_rand_clear(state);

    fq_gen(gen_sub, sub_ctx);
    fq_set(gen_sup, modulus->coeffs, sup_ctx);
    fq_neg(gen_sup, gen_sup, sup_ctx);

    fmpz_mod_poly_set(minpoly, fq_ctx_modulus(sub_ctx), sub_ctx->ctxp);

    fq_poly_clear(modulus, sup_ctx);
    fq_poly_clear(fact, sup_ctx);
}
