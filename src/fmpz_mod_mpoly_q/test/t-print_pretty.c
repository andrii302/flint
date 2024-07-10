/*
    Copyright (C) 2011 Sebastian Pancratz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

/*
    Example test program for the fmpz_mod_mpoly_q module.
*/

#include "stdlib.h"
#include "stdio.h"
#include "gmp.h"
#include "flint.h"
#include "fmpz.h"
#include "fmpz_mod.h"
#include "fmpz_mod_poly.h"
#include "fmpz_mod_mpoly.h"
#include "fmpz_mod_mpoly_q.h"


int main(int argc, char* argv[])
{
    fmpz_t n;
    fmpz_t p;
    fmpz_mod_mpoly_ctx_t ctx;
    fmpz_mod_mpoly_t x, y;
    const char * vars[] = { "x", "y", "z" };

    fmpz_init_set_ui(p, 7);
    fmpz_mod_mpoly_ctx_init(ctx, 3, ORD_LEX, p);
    fmpz_mod_mpoly_q_init(x, ctx);
    fmpz_mod_mpoly_q_init(y, ctx);

    // fmpz_mod_poly_set_coeff_ui(x, 3, 5, ctx); * Work in progress
    fmpz_mpoly_q_set_str_pretty(y, "(x^3+3*x*y^2+y*z)/(y^4+z^5)", vars, ctx);
    
    fmpz_mod_poly_print(x, ctx); flint_printf("\n");
    fmpz_mod_poly_print(y, ctx); flint_printf("\n");
    fmpz_mod_poly_clear(x, ctx);
    fmpz_mod_poly_clear(y, ctx);
    fmpz_mod_ctx_clear(ctx);
    fmpz_clear(n);

    return 0;
}