/*
    Copyright (C) 2011 Sebastian Pancratz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

/*
    Simple example demonstrating the use of the fmpz_mpoly_q module.
 */

#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>

#include <flint/flint.h>
#include <flint/fmpz.h>
#include <flint/fmpz_mpoly.h>
#include <flint/fmpz_mpoly_q.h>

int main(int argc, char* argv[])
{
    char *str, *strf, *strg;
    fmpz_t n;
    fmpz_mpoly_q_t f, g, h;
    fmpz_mpoly_ctx_t ctx;
    const char * vars[] = { "x", "y", "z" };

    fmpz_mpoly_ctx_init(ctx, 3, ORD_LEX);
    fmpz_mpoly_q_init(f, ctx);
    fmpz_mpoly_q_init(g, ctx);

    fmpz_mpoly_q_set_str_pretty(f, "(x^3+3*x*y^2+y*z)/(y^4+z^5)", vars, ctx);
    fmpz_mpoly_q_set_str_pretty(g, "(x*8+z^4)/(y+5)", vars, ctx); 
    fmpz_mpoly_q_print_pretty(f,vars, ctx); flint_printf("\n");
    strf = fmpz_mpoly_q_get_str_pretty(f, vars, ctx);
    strg = fmpz_mpoly_q_get_str_pretty(g, vars, ctx);
    fmpz_mpoly_q_mul(f, f, g, ctx);
    str  = fmpz_mpoly_q_get_str_pretty(f, vars, ctx);
    flint_printf("%s * %s = %s\n", strf, strg, str);
    flint_free(str);
    flint_free(strf);
    flint_free(strg);
    fmpz_mpoly_q_clear(f, ctx);
    fmpz_mpoly_q_clear(g, ctx);

    return 0;
}