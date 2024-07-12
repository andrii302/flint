/*
    Copyright (C) 2011 Sebastian Pancratz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

/*
    Example program for the fmpz_mod_mpoly module.
*/

#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include <flint/flint.h>
#include <flint/fmpz.h>
#include <flint/fmpz_mod_mpoly.h>
#include <flint/fmpz_mod_mpoly_q.h>

int main(int argc, char* argv[])
{
    char *str, *strf, *strg;
    fmpz_t n;
    fmpz_t p;
    fmpz_mod_mpoly_ctx_t ctx;
    fmpz_mod_mpoly_q_t x, y;
    const char * vars[] = { "x", "y", "z" };

    fmpz_init_set_ui(p, 7);
    fmpz_mod_mpoly_ctx_init(ctx, 3, ORD_LEX, p);
    fmpz_mod_mpoly_q_init(x, ctx);
    fmpz_mod_mpoly_q_init(y, ctx);

    fmpz_mod_mpoly_q_set_str_pretty(x, "(x^3+2*x*y^2+13*y*z)", vars, ctx);
    fmpz_mod_mpoly_q_print_pretty(x,vars, ctx); flint_printf("\n");
    fmpz_mod_mpoly_q_set_str_pretty(y, "(3*y^4+7*z^5)", vars, ctx);
    fmpz_mod_mpoly_q_mul(x, x, y, ctx);
    
    // strf = fmpz_mod_mpoly_q_get_str_pretty(x, vars, ctx);
    flint_printf("%s\n", strf);
    fmpz_mod_mpoly_q_clear(x, ctx);
    fmpz_mod_mpoly_q_clear(y, ctx);
    fmpz_mod_mpoly_ctx_clear(ctx);
    fmpz_clear(n);

    return 0;
}

