/*
    Copyright (C) 2015 Kushagra Singh

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "mpn_extras.h"
#include "fmpz_factor.h"

/* Select Montgomery Elliptic Curve given a sigma
   (Suyama's parameterization)
   Returns 1 in case factor is found while selecting
   the curve. */

/* Also selects initial point Q0 [x0 :: z0]  (z0 = 1) */

int
fmpz_factor_ecm_select_curve(nn_ptr f, nn_ptr sig, nn_ptr n, ecm_t ecm_inf)
{
    slong sz, cy;
    mp_size_t invlimbs;
    slong gcdlimbs;
    nn_ptr temp, tempv, tempn, tempi, tempf;
    int ret;

    TMP_INIT;

    TMP_START;
    temp = TMP_ALLOC(ecm_inf->n_size * sizeof(ulong));
    tempv = TMP_ALLOC((ecm_inf->n_size) * sizeof(ulong));
    tempn = TMP_ALLOC((ecm_inf->n_size) * sizeof(ulong));
    tempi = TMP_ALLOC((ecm_inf->n_size + 1) * sizeof(ulong));
    tempf = TMP_ALLOC((ecm_inf->n_size + 1) * sizeof(ulong));

    mpn_zero(tempn, ecm_inf->n_size);
    mpn_zero(tempv, ecm_inf->n_size);
    mpn_zero(temp, ecm_inf->n_size);
    flint_mpn_copyi(ecm_inf->u, sig, ecm_inf->n_size);

    temp[0] = UWORD(4);
    ret = 0;

    if (ecm_inf->normbits)
       mpn_lshift(temp, temp, ecm_inf->n_size, ecm_inf->normbits);   /* temp = (4 << norm) */

    flint_mpn_mulmod_preinvn(ecm_inf->v, ecm_inf->u, temp, ecm_inf->n_size,
                             n, ecm_inf->ninv, ecm_inf->normbits);

    flint_mpn_mulmod_preinvn(ecm_inf->w, ecm_inf->u, ecm_inf->u, ecm_inf->n_size,
                             n, ecm_inf->ninv, ecm_inf->normbits);

    mpn_add_n(temp, temp, ecm_inf->one, ecm_inf->n_size); /* temp = (5 << norm) */

    flint_mpn_submod_n(ecm_inf->u, ecm_inf->w, temp, n, ecm_inf->n_size);

    flint_mpn_mulmod_preinvn(ecm_inf->w, ecm_inf->u, ecm_inf->u, ecm_inf->n_size,
                             n, ecm_inf->ninv, ecm_inf->normbits);

    flint_mpn_mulmod_preinvn(ecm_inf->x, ecm_inf->w, ecm_inf->u, ecm_inf->n_size,
                             n, ecm_inf->ninv, ecm_inf->normbits);

    flint_mpn_mulmod_preinvn(ecm_inf->w, ecm_inf->v, ecm_inf->v, ecm_inf->n_size,
                             n, ecm_inf->ninv, ecm_inf->normbits);

    flint_mpn_mulmod_preinvn(ecm_inf->z, ecm_inf->w, ecm_inf->v, ecm_inf->n_size,
                             n, ecm_inf->ninv, ecm_inf->normbits);

    flint_mpn_mulmod_preinvn(ecm_inf->w, ecm_inf->x, ecm_inf->v, ecm_inf->n_size,
                             n, ecm_inf->ninv, ecm_inf->normbits);

    mpn_sub_n(temp, temp, ecm_inf->one, ecm_inf->n_size); /* temp = (4 << norm) */

    flint_mpn_mulmod_preinvn(ecm_inf->t, ecm_inf->w, temp, ecm_inf->n_size,
                             n, ecm_inf->ninv, ecm_inf->normbits);

    mpn_sub_n(temp, temp, ecm_inf->one, ecm_inf->n_size); /* temp = (3 << norm) */

    flint_mpn_mulmod_preinvn(ecm_inf->w, ecm_inf->u, temp, ecm_inf->n_size,
                             n, ecm_inf->ninv, ecm_inf->normbits);

    flint_mpn_submod_n(ecm_inf->u, ecm_inf->v, ecm_inf->u, n, ecm_inf->n_size);

    flint_mpn_addmod_n(ecm_inf->v, ecm_inf->v, ecm_inf->w, n, ecm_inf->n_size);

    flint_mpn_mulmod_preinvn(ecm_inf->w, ecm_inf->u, ecm_inf->u, ecm_inf->n_size,
                             n, ecm_inf->ninv, ecm_inf->normbits);

    flint_mpn_mulmod_preinvn(ecm_inf->u, ecm_inf->u, ecm_inf->w, ecm_inf->n_size,
                             n, ecm_inf->ninv, ecm_inf->normbits);

    flint_mpn_mulmod_preinvn(ecm_inf->a24, ecm_inf->u, ecm_inf->v, ecm_inf->n_size,
                             n, ecm_inf->ninv, ecm_inf->normbits);

    flint_mpn_mulmod_preinvn(ecm_inf->v, ecm_inf->t, ecm_inf->z, ecm_inf->n_size,
                             n, ecm_inf->ninv, ecm_inf->normbits);

    sz = ecm_inf->n_size;
    MPN_NORM(ecm_inf->v, sz);  /* sz has number of limbs of v */

    if (sz == 0)        /* v = 0, gcd(0, n) = n. Hence inverse will not exist. */
    {                   /* No point in further computation with curve */
        ret = (-1);
        goto cleanup;
    }

    flint_mpn_copyi(tempv, ecm_inf->v, sz);
    flint_mpn_copyi(tempn, n, ecm_inf->n_size);

    /* NOTE: invlimbs must be mp_size_t since it is strictly different from
     * slong on Windows systems. */
    gcdlimbs = mpn_gcdext(tempf, tempi, &invlimbs, tempv, sz, tempn, ecm_inf->n_size);

    if (!(gcdlimbs == 1 && tempf[0] == ecm_inf->one[0]) &&
        !(gcdlimbs == (slong) ecm_inf->n_size && mpn_cmp(tempf, n, ecm_inf->n_size) == 0))
    {
        /* Found factor */
        flint_mpn_copyi(f, tempf, gcdlimbs);
        ret = gcdlimbs;
        goto cleanup;
    }

    if (invlimbs < 0) /* negative inverse, add n */
    {
        invlimbs *= -1;

        if (ecm_inf->normbits)
        {
            cy = mpn_lshift(tempi, tempi, invlimbs, ecm_inf->normbits);
            if (cy)
                tempi[invlimbs] = cy;
        }

        mpn_sub_n(tempi, n, tempi, ecm_inf->n_size);\
    }
    else
    {
        if (ecm_inf->normbits)
        {
            cy = mpn_lshift(tempi, tempi, invlimbs, ecm_inf->normbits);
            if (cy)
                tempi[invlimbs] = cy;
        }
    }

    MPN_NORM(tempi, invlimbs);
    mpn_zero(ecm_inf->u, ecm_inf->n_size);
    flint_mpn_copyi(ecm_inf->u, tempi, invlimbs);

    flint_mpn_mulmod_preinvn(ecm_inf->v, ecm_inf->u, ecm_inf->t, ecm_inf->n_size,
                             n, ecm_inf->ninv, ecm_inf->normbits);

    flint_mpn_mulmod_preinvn(ecm_inf->x, ecm_inf->x, ecm_inf->v, ecm_inf->n_size,
                             n, ecm_inf->ninv, ecm_inf->normbits);

    flint_mpn_mulmod_preinvn(ecm_inf->v, ecm_inf->u, ecm_inf->z, ecm_inf->n_size,
                             n, ecm_inf->ninv, ecm_inf->normbits);

    flint_mpn_mulmod_preinvn(ecm_inf->w, ecm_inf->a24, ecm_inf->v, ecm_inf->n_size,
                             n, ecm_inf->ninv, ecm_inf->normbits);

    mpn_zero(temp, sz);
    temp[0] = UWORD(2);
    if (ecm_inf->normbits)
        mpn_lshift(temp, temp, ecm_inf->n_size, ecm_inf->normbits);

    flint_mpn_addmod_n(ecm_inf->a24, ecm_inf->w, temp, n, ecm_inf->n_size);
    mpn_rshift(ecm_inf->a24, ecm_inf->a24, ecm_inf->n_size, 2);
    if (ecm_inf->normbits)
       ecm_inf->a24[0] &= ~((UWORD(1)<<ecm_inf->normbits) - 1);

    flint_mpn_copyi(ecm_inf->z, ecm_inf->one, ecm_inf->n_size);

    cleanup:

    TMP_END;

    return ret;
}
