/*
    Copyright (C) 2011 Sebastian Pancratz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include <gmp.h>
#include "fmpq.h"
#include "padic.h"

void padic_get_mpq(mpq_t rop, const padic_t op, const padic_ctx_t ctx)
{
    fmpq_t t;

    fmpq_init(t);
    padic_get_fmpq(t, op, ctx);
    fmpq_get_mpq(rop, t);
    fmpq_clear(t);
}
