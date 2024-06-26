/*
    Copyright (C) 2019 Daniel Schultz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "fq_nmod.h"
#include "n_poly.h"
#include "mpoly.h"
#include "fq_nmod_mpoly.h"

void fq_nmod_mpoly_add_n_fq(
    fq_nmod_mpoly_t A,
    const fq_nmod_mpoly_t B,
    const ulong * c,
    const fq_nmod_mpoly_ctx_t ctx)
{
    slong d = fq_nmod_ctx_degree(ctx->fqctx);
    slong N = mpoly_words_per_exp(B->bits, ctx->minfo);
    slong Blen = B->length;

    if (_n_fq_is_zero(c, d))
    {
        fq_nmod_mpoly_set(A, B, ctx);
        return;
    }

    if (Blen < 1)
    {
        fq_nmod_mpoly_set_n_fq(A, c, ctx);
        return;
    }

    if (mpoly_monomial_is_zero(B->exps + (Blen - 1)*N, N))
    {
        if (A != B)
        {
            fq_nmod_mpoly_fit_length_reset_bits(A, B->length, B->bits, ctx);
            _nmod_vec_set(A->coeffs, B->coeffs, d*(Blen - 1));
            mpoly_copy_monomials(A->exps, B->exps, Blen, N);
            _fq_nmod_mpoly_set_length(A, B->length, ctx);
        }

        n_fq_add(A->coeffs + d*(Blen - 1), B->coeffs + d*(Blen - 1), c, ctx->fqctx);
        if (_n_fq_is_zero(A->coeffs + d*(Blen - 1), d))
            _fq_nmod_mpoly_set_length(A, Blen - 1, ctx);
    }
    else
    {
        if (A != B)
        {
            fq_nmod_mpoly_fit_length_reset_bits(A, Blen + 1, B->bits, ctx);
            _nmod_vec_set(A->coeffs, B->coeffs, d*Blen);
            mpoly_copy_monomials(A->exps, B->exps, Blen, N);
        }
        else
        {
            fq_nmod_mpoly_fit_length(A, Blen + 1, ctx);
        }

        mpoly_monomial_zero(A->exps + N*Blen, N);
        _n_fq_set(A->coeffs + d*Blen, c, d);
        _fq_nmod_mpoly_set_length(A, Blen + 1, ctx);
    }
}


void fq_nmod_mpoly_add_fq_nmod(
    fq_nmod_mpoly_t A,
    const fq_nmod_mpoly_t B,
    const fq_nmod_t c,
    const fq_nmod_mpoly_ctx_t ctx)
{
    slong d = fq_nmod_ctx_degree(ctx->fqctx);
    slong N = mpoly_words_per_exp(B->bits, ctx->minfo);
    slong Blen = B->length;

    if (fq_nmod_is_zero(c, ctx->fqctx))
    {
        fq_nmod_mpoly_set(A, B, ctx);
        return;
    }

    if (Blen < 1)
    {
        fq_nmod_mpoly_set_fq_nmod(A, c, ctx);
        return;
    }

    if (mpoly_monomial_is_zero(B->exps + (Blen - 1)*N, N))
    {
        if (A != B)
        {
            fq_nmod_mpoly_fit_length_reset_bits(A, B->length, B->bits, ctx);
            _nmod_vec_set(A->coeffs, B->coeffs, d*(Blen - 1));
            mpoly_copy_monomials(A->exps, B->exps, Blen, N);
            _fq_nmod_mpoly_set_length(A, B->length, ctx);
        }

        n_fq_add_fq_nmod(A->coeffs + d*(Blen - 1), B->coeffs + d*(Blen - 1), c, ctx->fqctx);
        if (_n_fq_is_zero(A->coeffs + d*(Blen - 1), d))
            _fq_nmod_mpoly_set_length(A, Blen - 1, ctx);
    }
    else
    {
        if (A != B)
        {
            fq_nmod_mpoly_fit_length_reset_bits(A, Blen + 1, B->bits, ctx);
            _nmod_vec_set(A->coeffs, B->coeffs, d*Blen);
            mpoly_copy_monomials(A->exps, B->exps, Blen, N);
        }
        else
        {
            fq_nmod_mpoly_fit_length(A, Blen + 1, ctx);
        }

        mpoly_monomial_zero(A->exps + N*Blen, N);
        n_fq_set_fq_nmod(A->coeffs + d*Blen, c, ctx->fqctx);
        _fq_nmod_mpoly_set_length(A, Blen + 1, ctx);
    }
}
