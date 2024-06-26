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

void flint_mpq_clear_readonly(mpq_t z)
{
    flint_mpz_clear_readonly(mpq_numref(z));
    flint_mpz_clear_readonly(mpq_denref(z));
}
