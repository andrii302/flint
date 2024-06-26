/*
    Copyright (C) 2010 William Hart

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "nmod.h"
#include "nmod_vec.h"

void _nmod_vec_add(nn_ptr res, nn_srcptr vec1,
                   nn_srcptr vec2, slong len, nmod_t mod)
{
    slong i;

    if (mod.norm)
    {
        for (i = 0 ; i < len; i++)
        res[i] = _nmod_add(vec1[i], vec2[i], mod);
    } else
    {
        for (i = 0 ; i < len; i++)
        res[i] = nmod_add(vec1[i], vec2[i], mod);
    }
}
