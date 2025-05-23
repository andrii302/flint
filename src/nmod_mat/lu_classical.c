/*
    Copyright (C) 2011 Fredrik Johansson

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "nmod.h"
#include "nmod_vec.h"
#include "nmod_mat.h"

static inline int
nmod_mat_pivot(nmod_mat_t A, slong * P, slong start_row, slong col)
{
    slong j;

    if (nmod_mat_entry(A, start_row, col) != 0)
        return 1;

    for (j = start_row + 1; j < A->r; j++)
    {
        if (nmod_mat_entry(A, j, col) != 0)
        {
            nmod_mat_swap_rows(A, P, j, start_row);
            return -1;
        }
    }
    return 0;
}


slong
nmod_mat_lu_classical(slong * P, nmod_mat_t A, int rank_check)
{
    ulong d, e;
    nmod_t mod;
    slong i, m, n, rank, length, row, col;

    m = A->r;
    n = A->c;
    mod = A->mod;

    rank = row = col = 0;

    for (i = 0; i < m; i++)
        P[i] = i;

    while (row < m && col < n)
    {
        if (nmod_mat_pivot(A, P, row, col) == 0)
        {
            if (rank_check)
                return 0;
            col++;
            continue;
        }

        rank++;

        d = nmod_mat_entry(A, row, col);
        d = nmod_inv(d, mod);
        length = n - col - 1;

        for (i = row + 1; i < m; i++)
        {
            e = nmod_mul(nmod_mat_entry(A, i, col), d, mod);
            if (length != 0)
                _nmod_vec_scalar_addmul_nmod(nmod_mat_entry_ptr(A, i, col + 1),
                    nmod_mat_entry_ptr(A, row, + col + 1), length, nmod_neg(e, mod), mod);

            nmod_mat_entry(A, i, col) = 0;
            nmod_mat_entry(A, i, rank - 1) = e;
        }
        row++;
        col++;
    }

    return rank;
}
