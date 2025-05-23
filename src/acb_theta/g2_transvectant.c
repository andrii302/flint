/*
    Copyright (C) 2023 Jean Kieffer

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "acb_poly.h"
#include "acb_theta.h"

static void
acb_theta_g2_transvectant_lead(acb_t res, const acb_poly_t g, const acb_poly_t h,
    slong m, slong n, slong k, slong prec)
{
    acb_ptr s, t;
    fmpz_t num, f;
    slong j;

    s = _acb_vec_init(k + 1);
    t = _acb_vec_init(k + 1);
    fmpz_init(num);
    fmpz_init(f);

    /* Set i = m - k (resp. n - k) in g2_transvectant and use acb_dot */
    for (j = 0; j <= k; j++)
    {
        acb_poly_get_coeff_acb(&s[j], g, m - j);
        acb_poly_get_coeff_acb(&t[j], h, n - k + j);
        /* Put all factorials in s */
        fmpz_fac_ui(num, m - j);
        fmpz_fac_ui(f, n - k + j);
        fmpz_mul(num, num, f);
        if ((k - j) % 2 == 1)
        {
            fmpz_neg(num, num);
        }
        acb_mul_fmpz(&s[j], &s[j], num, prec);
    }
    acb_dot(res, NULL, 0, s, 1, t, 1, k + 1, prec);

    fmpz_fac_ui(num, k);
    acb_set_fmpz(t, num);
    fmpz_fac_ui(num, m);
    fmpz_fac_ui(f, n);
    fmpz_mul(num, num, f);
    acb_div_fmpz(t, t, num, prec);
    acb_mul(res, res, t, prec);

    _acb_vec_clear(s, k + 1);
    _acb_vec_clear(t, k + 1);
    fmpz_clear(num);
    fmpz_clear(f);
}

static void
acb_theta_g2_transvectant_poly(acb_poly_t res, const acb_poly_t g, const acb_poly_t h,
    slong m, slong n, slong k, slong prec)
{
    acb_poly_t aux, s, t;
    acb_t x;
    fmpz_t num, f;
    slong i, j;

    acb_poly_init(aux);
    acb_poly_init(s);
    acb_poly_init(t);
    acb_init(x);
    fmpz_init(num);
    fmpz_init(f);

    for (j = 0; j <= k; j++)
    {
        /* Set s to d^k g / dx^{k-j} dy^j; g was of degree m */
        acb_poly_zero(s);
        for (i = 0; i <= m - k; i++)
        {
            fmpz_fac_ui(num, i + (k - j));
            fmpz_fac_ui(f, (m - k - i) + j);
            fmpz_mul(num, num, f);
            fmpz_bin_uiui(f, m - k, i);
            fmpz_mul(num, num, f);

            acb_poly_get_coeff_acb(x, g, i + (k - j));
            acb_mul_fmpz(x, x, num, prec);
            acb_poly_set_coeff_acb(s, i, x);
        }

        /* Set t to d^k h / dx^j dy^{k-j}; h was of degree n */
        acb_poly_zero(t);
        for (i = 0; i <= n - k; i++)
        {
            fmpz_fac_ui(num, i + j);
            fmpz_fac_ui(f, (n - k - i) + (k - j));
            fmpz_mul(num, num, f);
            fmpz_bin_uiui(f, n - k, i);
            fmpz_mul(num, num, f);

            acb_poly_get_coeff_acb(x, h, i + j);
            acb_mul_fmpz(x, x, num, prec);
            acb_poly_set_coeff_acb(t, i, x);
        }

        acb_poly_mul(s, s, t, prec);
        fmpz_bin_uiui(f, k, j);
        if ((k - j) % 2 == 1)
        {
            fmpz_neg(f, f);
        }
        acb_set_fmpz(x, f);
        acb_poly_scalar_mul(s, s, x, prec);
        acb_poly_add(aux, aux, s, prec);
    }

    fmpz_fac_ui(num, m);
    fmpz_fac_ui(f, n);
    fmpz_mul(num, num, f);

    acb_one(x);
    acb_div_fmpz(x, x, num, prec);
    acb_poly_scalar_mul(res, aux, x, prec);

    acb_poly_clear(aux);
    acb_poly_clear(s);
    acb_poly_clear(t);
    acb_clear(x);
    fmpz_clear(num);
    fmpz_clear(f);
}

void
acb_theta_g2_transvectant(acb_poly_t res, const acb_poly_t g, const acb_poly_t h,
    slong m, slong n, slong k, int lead, slong prec)
{
    if (lead)
    {
        acb_t x;
        acb_init(x);

        acb_theta_g2_transvectant_lead(x, g, h, m, n, k, prec);
        acb_poly_zero(res);
        acb_poly_set_coeff_acb(res, 0, x);

        acb_clear(x);
    }
    else
    {
        acb_theta_g2_transvectant_poly(res, g, h, m, n, k, prec);
    }
}
