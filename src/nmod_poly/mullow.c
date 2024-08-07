/*
    Copyright (C) 2010 William Hart
    Copyright (C) 2021 Fredrik Johansson

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "nmod.h"
#include "nmod_poly.h"

#ifdef FLINT_HAVE_FFT_SMALL

#include "fft_small.h"

/* todo: separate squaring table */
/* todo: check unbalanced cutoffs */

static const short fft_mullow_tab[] = {1115, 1115, 597, 569, 407, 321, 306, 279, 191,
182, 166, 159, 152, 145, 139, 89, 85, 78, 75, 75, 69, 174, 174, 166, 159,
152, 152, 152, 97, 101, 106, 111, 101, 101, 101, 139, 145, 145, 139, 145,
145, 139, 145, 145, 145, 182, 182, 182, 182, 182, 182, 191, 200, 220, 210,
200, 210, 210, 210, 210, 191, 182, 182, 174, };

#endif


void _nmod_poly_mullow(mp_ptr res, mp_srcptr poly1, slong len1,
                             mp_srcptr poly2, slong len2, slong n, nmod_t mod)
{
    slong bits;

    len1 = FLINT_MIN(len1, n);
    len2 = FLINT_MIN(len2, n);

    if (len2 <= 5)
    {
        _nmod_poly_mullow_classical(res, poly1, len1, poly2, len2, n, mod);
        return;
    }

    if (n == len1 + len2 - 1)
    {
        _nmod_poly_mul(res, poly1, len1, poly2, len2, mod);
        return;
    }

    bits = NMOD_BITS(mod);

#ifdef FLINT_HAVE_FFT_SMALL

    if (len2 >= fft_mullow_tab[bits - 1])
    {
        _nmod_poly_mul_mid_default_mpn_ctx(res, 0, n, poly1, len1, poly2, len2, mod);
        return;
    }

#endif

    if (n < 10 + bits * bits / 10)
        _nmod_poly_mullow_classical(res, poly1, len1, poly2, len2, n, mod);
    else
        _nmod_poly_mullow_KS(res, poly1, len1, poly2, len2, 0, n, mod);
}

void nmod_poly_mullow(nmod_poly_t res,
              const nmod_poly_t poly1, const nmod_poly_t poly2, slong trunc)
{
    slong len1, len2, len_out;

    len1 = poly1->length;
    len2 = poly2->length;

    len_out = poly1->length + poly2->length - 1;
    if (trunc > len_out)
        trunc = len_out;

    if (len1 == 0 || len2 == 0 || trunc == 0)
    {
        nmod_poly_zero(res);

        return;
    }

    if (res == poly1 || res == poly2)
    {
        nmod_poly_t temp;

        nmod_poly_init2(temp, poly1->mod.n, trunc);

        if (len1 >= len2)
            _nmod_poly_mullow(temp->coeffs, poly1->coeffs, len1,
                           poly2->coeffs, len2, trunc, poly1->mod);
        else
            _nmod_poly_mullow(temp->coeffs, poly2->coeffs, len2,
                           poly1->coeffs, len1, trunc, poly1->mod);

        nmod_poly_swap(temp, res);
        nmod_poly_clear(temp);
    } else
    {
        nmod_poly_fit_length(res, trunc);

        if (len1 >= len2)
            _nmod_poly_mullow(res->coeffs, poly1->coeffs, len1,
                           poly2->coeffs, len2, trunc, poly1->mod);
        else
            _nmod_poly_mullow(res->coeffs, poly2->coeffs, len2,
                           poly1->coeffs, len1, trunc, poly1->mod);
    }

    res->length = trunc;
    _nmod_poly_normalise(res);
}
