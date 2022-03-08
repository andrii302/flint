#include "gr.h"

/* Generic arithmetic functions */

/* Assumes exp >= 2; res and tmp not not aliased with x. */
static int
_gr_generic_pow_ui_binexp(gr_ptr res, gr_ptr tmp, gr_srcptr x, ulong exp, gr_ctx_t ctx)
{
    gr_ptr R, S, T;
    int status;
    int zeros;
    ulong bit;

    status = GR_SUCCESS;

    /* Determine parity due to swaps */
    zeros = 0;
    bit = exp;
    while (bit > 1)
    {
        zeros += !(bit & 1);
        bit >>= 1;
    }

    if (zeros % 2)
    {
        R = res;
        S = tmp;
    }
    else
    {
        R = tmp;
        S = res;
    }

    bit = UWORD(1) << (FLINT_BIT_COUNT(exp) - 2);

    status |= gr_mul(R, x, x, ctx);

    if (bit & exp)
    {
        status |= gr_mul(S, R, x, ctx);
        T = R;
        R = S;
        S = T;
    }

    while (bit >>= 1)
    {
        status |= gr_mul(S, R, R, ctx);

        if (bit & exp)
        {
            status |= gr_mul(R, S, x, ctx);
        }
        else
        {
            T = R;
            R = S;
            S = T;
        }
    }

    return status;
}

int
gr_generic_pow_ui(gr_ptr res, gr_srcptr x, ulong e, gr_ctx_t ctx)
{
    int status;

    if (e == 0)
    {
        return gr_one(res, ctx);
    }
    else if (e == 1)
    {
        return gr_set(res, x, ctx);
    }
    else if (e == 2)
    {
        return gr_mul(res, x, x, ctx);
    }
    else if (e == 4)
    {
        status = gr_mul(res, x, x, ctx);
        status |= gr_mul(res, res, res, ctx);
        return status;
    }
    else
    {
        gr_ptr t, u;
        GR_TMP_START;

        if (res == x)
        {
            GR_TMP_INIT2(t, u, ctx);
            gr_set(u, x, ctx);
            status = _gr_generic_pow_ui_binexp(res, t, u, e, ctx);
            GR_TMP_CLEAR2(t, u, ctx);
        }
        else
        {
            GR_TMP_INIT1(t, ctx);
            status = _gr_generic_pow_ui_binexp(res, t, x, e, ctx);
            GR_TMP_CLEAR1(t, ctx);
        }

        GR_TMP_END;
        return status;
    }
}

/* todo: call gr_pow_ui instead of gr_generic_pow_ui? */
int
gr_generic_pow_si(gr_ptr res, gr_srcptr x, slong e, gr_ctx_t ctx)
{
    if (e >= 0)
    {
        return gr_generic_pow_ui(res, x, e, ctx);
    }
    else
    {
        int status;

        /* todo: some heuristic for when we want to invert before/after powering */
        status = gr_inv(res, x, ctx);
        if (status == GR_SUCCESS)
            status = gr_generic_pow_ui(res, x, -e, ctx);

        return status;
    }
}

/* Generic vector functions */

int
gr_generic_vec_init(gr_ptr vec, slong len, gr_ctx_t ctx)
{
    int status;
    slong i, sz;

    sz = ctx->sizeof_elem;
    status = GR_SUCCESS;

    for (i = 0; i < len; i++)
        status |= gr_init(GR_ENTRY(vec, i, sz), ctx);

    return status;
}

int
gr_generic_vec_clear(gr_ptr vec, slong len, gr_ctx_t ctx)
{
    int status;
    slong i, sz;

    sz = ctx->sizeof_elem;
    status = GR_SUCCESS;

    for (i = 0; i < len; i++)
        status |= gr_clear(GR_ENTRY(vec, i, sz), ctx);

    return status;
}

int
gr_generic_vec_swap(gr_ptr vec1, gr_ptr vec2, slong len, gr_ctx_t ctx)
{
    int status;
    slong i, sz;

    sz = ctx->sizeof_elem;
    status = GR_SUCCESS;

    for (i = 0; i < len; i++)
        status |= gr_swap(GR_ENTRY(vec1, i, sz), GR_ENTRY(vec2, i, sz), ctx);

    return status;
}

int
gr_generic_vec_zero(gr_ptr vec, slong len, gr_ctx_t ctx)
{
    int status;
    slong i, sz;

    sz = ctx->sizeof_elem;
    status = GR_SUCCESS;

    for (i = 0; i < len; i++)
        status |= gr_zero(GR_ENTRY(vec, i, sz), ctx);

    return status;
}

int
gr_generic_vec_set(gr_ptr res, gr_srcptr src, slong len, gr_ctx_t ctx)
{
    int status;
    slong i, sz;

    sz = ctx->sizeof_elem;
    status = GR_SUCCESS;

    for (i = 0; i < len; i++)
        status |= gr_set(GR_ENTRY(res, i, sz), GR_ENTRY(src, i, sz), ctx);

    return status;
}

int
gr_generic_vec_neg(gr_ptr res, gr_srcptr src, slong len, gr_ctx_t ctx)
{
    int status;
    slong i, sz;

    sz = ctx->sizeof_elem;
    status = GR_SUCCESS;

    for (i = 0; i < len; i++)
        status |= gr_neg(GR_ENTRY(res, i, sz), GR_ENTRY(src, i, sz), ctx);

    return status;
}

int
gr_generic_vec_add(gr_ptr res, gr_srcptr src1, gr_srcptr src2, slong len, gr_ctx_t ctx)
{
    int status;
    slong i, sz;

    sz = ctx->sizeof_elem;
    status = GR_SUCCESS;

    for (i = 0; i < len; i++)
        status |= gr_add(GR_ENTRY(res, i, sz), GR_ENTRY(src1, i, sz), GR_ENTRY(src2, i, sz), ctx);

    return status;
}

int
gr_generic_vec_sub(gr_ptr res, gr_srcptr src1, gr_srcptr src2, slong len, gr_ctx_t ctx)
{
    int status;
    slong i, sz;

    sz = ctx->sizeof_elem;
    status = GR_SUCCESS;

    for (i = 0; i < len; i++)
        status |= gr_sub(GR_ENTRY(res, i, sz), GR_ENTRY(src1, i, sz), GR_ENTRY(src2, i, sz), ctx);

    return status;
}

int
gr_generic_vec_scalar_addmul(gr_ptr vec1, gr_srcptr vec2, slong len, gr_srcptr c, gr_ctx_t ctx)
{
    GR_TMP_START;
    int status;
    slong i, sz;
    gr_ptr t;

    sz = ctx->sizeof_elem;
    status = GR_SUCCESS;

    GR_TMP_INIT1(t, ctx);

    for (i = 0; i < len; i++)
    {
        status |= gr_mul(t, GR_ENTRY(vec2, i, sz), c, ctx);
        status |= gr_add(GR_ENTRY(vec1, i, sz), GR_ENTRY(vec1, i, sz), t, ctx);
    }

    GR_TMP_CLEAR1(t, ctx);
    GR_TMP_END;
    return status;
}

int
gr_generic_vec_scalar_submul(gr_ptr vec1, gr_srcptr vec2, slong len, gr_srcptr c, gr_ctx_t ctx)
{
    GR_TMP_START;
    int status;
    slong i, sz;
    gr_ptr t;

    sz = ctx->sizeof_elem;
    status = GR_SUCCESS;

    GR_TMP_INIT1(t, ctx);

    for (i = 0; i < len; i++)
    {
        status |= gr_mul(t, GR_ENTRY(vec2, i, sz), c, ctx);
        status |= gr_sub(GR_ENTRY(vec1, i, sz), GR_ENTRY(vec1, i, sz), t, ctx);
    }

    GR_TMP_CLEAR1(t, ctx);
    GR_TMP_END;
    return status;
}

int
gr_generic_vec_scalar_addmul_si(gr_ptr vec1, gr_srcptr vec2, slong len, slong c, gr_ctx_t ctx)
{
    GR_TMP_START;
    int status;
    slong i, sz;
    gr_ptr t;

    sz = ctx->sizeof_elem;
    status = GR_SUCCESS;

    GR_TMP_INIT1(t, ctx);

    for (i = 0; i < len; i++)
    {
        status |= gr_mul_si(t, GR_ENTRY(vec2, i, sz), c, ctx);
        status |= gr_add(GR_ENTRY(vec1, i, sz), GR_ENTRY(vec1, i, sz), t, ctx);
    }

    GR_TMP_CLEAR1(t, ctx);
    GR_TMP_END;
    return status;
}

int
gr_generic_vec_scalar_submul_si(gr_ptr vec1, gr_srcptr vec2, slong len, slong c, gr_ctx_t ctx)
{
    GR_TMP_START;
    int status;
    slong i, sz;
    gr_ptr t;

    sz = ctx->sizeof_elem;
    status = GR_SUCCESS;

    GR_TMP_INIT1(t, ctx);

    for (i = 0; i < len; i++)
    {
        status |= gr_mul_si(t, GR_ENTRY(vec2, i, sz), c, ctx);
        status |= gr_sub(GR_ENTRY(vec1, i, sz), GR_ENTRY(vec1, i, sz), t, ctx);
    }

    GR_TMP_CLEAR1(t, ctx);
    GR_TMP_END;
    return status;
}

int
gr_generic_vec_equal(int * res, gr_srcptr vec1, gr_srcptr vec2, slong len, gr_ctx_t ctx)
{
    int status, equal, this_equal;
    slong i, sz;

    sz = ctx->sizeof_elem;
    status = GR_SUCCESS;

    equal = 1;
    for (i = 0; i < len; i++)
    {
        status |= gr_equal(&this_equal, GR_ENTRY(vec1, i, sz), GR_ENTRY(vec2, i, sz), ctx);
        equal = equal && this_equal;
    }

    res[0] = equal;
    return status;
}

int
gr_generic_vec_is_zero(int * res, gr_srcptr vec, slong len, gr_ctx_t ctx)
{
    int status, equal, this_equal;
    slong i, sz;

    sz = ctx->sizeof_elem;
    status = GR_SUCCESS;

    equal = 1;
    for (i = 0; i < len; i++)
    {
        status |= gr_is_zero(&this_equal, GR_ENTRY(vec, i, sz), ctx);
        equal = equal && this_equal;
    }

    res[0] = equal;
    return status;
}

int
gr_generic_vec_dot(gr_ptr res, gr_srcptr initial, int subtract, gr_srcptr vec1, gr_srcptr vec2, slong len, gr_ctx_t ctx)
{
    GR_TMP_START;
    int status;
    slong i, sz;
    gr_ptr t;

    if (len <= 0)
    {
        if (initial == NULL)
            return gr_zero(res, ctx);
        else
            return gr_set(res, initial, ctx);
    }

    sz = ctx->sizeof_elem;
    status = GR_SUCCESS;

    GR_TMP_INIT1(t, ctx);

    if (initial == NULL)
    {
        status |= gr_mul(res, vec1, vec2, ctx);
    }
    else
    {
        if (subtract)
            status |= gr_neg(res, initial, ctx);
        else
            status |= gr_set(res, initial, ctx);

        status |= gr_mul(t, vec1, vec2, ctx);
        status |= gr_add(res, res, t, ctx);
    }

    for (i = 1; i < len; i++)
    {
        status |= gr_mul(t, GR_ENTRY(vec1, i, sz), GR_ENTRY(vec2, i, sz), ctx);
        status |= gr_add(res, res, t, ctx);
    }

    if (subtract)
        status |= gr_neg(res, res, ctx);

    GR_TMP_CLEAR1(t, ctx);
    GR_TMP_END;
    return status;
}

int
gr_generic_vec_dot_rev(gr_ptr res, gr_srcptr initial, int subtract, gr_srcptr vec1, gr_srcptr vec2, slong len, gr_ctx_t ctx)
{
    GR_TMP_START;
    int status;
    slong i, sz;
    gr_ptr t;

    if (len <= 0)
    {
        if (initial == NULL)
            return gr_zero(res, ctx);
        else
            return gr_set(res, initial, ctx);
    }

    sz = ctx->sizeof_elem;
    status = GR_SUCCESS;

    GR_TMP_INIT1(t, ctx);

    if (initial == NULL)
    {
        status |= gr_mul(res, vec1, GR_ENTRY(vec2, len - 1, sz), ctx);
    }
    else
    {
        if (subtract)
            status |= gr_neg(res, initial, ctx);
        else
            status |= gr_set(res, initial, ctx);

        status |= gr_mul(t, vec1, GR_ENTRY(vec2, len - 1, sz), ctx);
        status |= gr_add(res, res, t, ctx);
    }

    for (i = 1; i < len; i++)
    {
        status |= gr_mul(t, GR_ENTRY(vec1, i, sz), GR_ENTRY(vec2, len - 1 - i, sz), ctx);
        status |= gr_add(res, res, t, ctx);
    }

    if (subtract)
        status |= gr_neg(res, res, ctx);

    GR_TMP_CLEAR1(t, ctx);
    GR_TMP_END;
    return status;
}

/* Generic method implementations */
const gr_method_tab_input gr_generic_methods[] =
{
    {GR_METHOD_POW_SI,                  (gr_funcptr) gr_generic_pow_si},
    {GR_METHOD_POW_UI,                  (gr_funcptr) gr_generic_pow_ui},

    {GR_METHOD_VEC_INIT,                (gr_funcptr) gr_generic_vec_init},
    {GR_METHOD_VEC_CLEAR,               (gr_funcptr) gr_generic_vec_clear},
    {GR_METHOD_VEC_SWAP,                (gr_funcptr) gr_generic_vec_swap},
    {GR_METHOD_VEC_ZERO,                (gr_funcptr) gr_generic_vec_zero},
    {GR_METHOD_VEC_SET,                 (gr_funcptr) gr_generic_vec_set},
    {GR_METHOD_VEC_NEG,                 (gr_funcptr) gr_generic_vec_neg},
    {GR_METHOD_VEC_ADD,                 (gr_funcptr) gr_generic_vec_add},
    {GR_METHOD_VEC_SUB,                 (gr_funcptr) gr_generic_vec_sub},
    {GR_METHOD_VEC_SCALAR_ADDMUL,       (gr_funcptr) gr_generic_vec_scalar_addmul},
    {GR_METHOD_VEC_SCALAR_SUBMUL,       (gr_funcptr) gr_generic_vec_scalar_submul},
    {GR_METHOD_VEC_SCALAR_ADDMUL_SI,    (gr_funcptr) gr_generic_vec_scalar_addmul_si},
    {GR_METHOD_VEC_SCALAR_SUBMUL_SI,    (gr_funcptr) gr_generic_vec_scalar_submul_si},
    {GR_METHOD_VEC_EQUAL,               (gr_funcptr) gr_generic_vec_equal},
    {GR_METHOD_VEC_IS_ZERO,             (gr_funcptr) gr_generic_vec_is_zero},
    {GR_METHOD_VEC_DOT,                 (gr_funcptr) gr_generic_vec_dot},
    {GR_METHOD_VEC_DOT_REV,             (gr_funcptr) gr_generic_vec_dot_rev},
    {0,                                 (gr_funcptr) NULL},
};

void
gr_method_tab_init(gr_method_tab_t * methods, gr_method_tab_input * tab)
{
    slong i;

    methods->methods = flint_malloc(sizeof(void *) * GR_METHOD_TAB_SIZE);

    for (i = 0; i < GR_METHOD_TAB_SIZE; i++)
        methods->methods[i] = (gr_funcptr) gr_not_implemented;

    /* Assign generic methods as fallbacks */
    for (i = 0; ; i++)
    {
        if (gr_generic_methods[i].function == NULL)
            break;

        if (gr_generic_methods[i].index >= GR_METHOD_TAB_SIZE)
            abort();

        methods->methods[gr_generic_methods[i].index] = gr_generic_methods[i].function;
    }

    for (i = 0; ; i++)
    {
        if (tab[i].function == NULL)
            break;

        if (tab[i].index >= GR_METHOD_TAB_SIZE)
            abort();

        methods->methods[tab[i].index] = tab[i].function;
    }
}



/* todo: thread safe */
int _matrix_methods2_initialized = 0;
gr_method_tab_t _matrix_methods2;

gr_method_tab_input matrix_methods2[] =
{
    {GR_METHOD_INIT,        (gr_funcptr) matrix_init},
    {GR_METHOD_CLEAR,       (gr_funcptr) matrix_clear},
    {GR_METHOD_RANDTEST,    (gr_funcptr) matrix_randtest},
    {GR_METHOD_WRITE,       (gr_funcptr) matrix_write},
    {GR_METHOD_ZERO,        (gr_funcptr) matrix_zero},
    {GR_METHOD_ONE,         (gr_funcptr) matrix_one},
    {GR_METHOD_EQUAL,       (gr_funcptr) matrix_equal},
    {GR_METHOD_SET,         (gr_funcptr) matrix_set},
    {GR_METHOD_NEG,         (gr_funcptr) matrix_neg},
    {GR_METHOD_ADD,         (gr_funcptr) matrix_add},
    {GR_METHOD_SUB,         (gr_funcptr) matrix_sub},
    {GR_METHOD_MUL,         (gr_funcptr) matrix_mul},
    {0,                     (gr_funcptr) NULL},
};

/* rename: gr_ctx_init_gr_mat */
void
gr_ctx_init_matrix(gr_ctx_t ctx, gr_ctx_t base_ring, slong n)
{
    ctx->flags = 0;
    ctx->sizeof_elem = sizeof(gr_mat_struct);
    ctx->elem_ctx = flint_malloc(sizeof(matrix_ctx_t));
    ((matrix_ctx_t *) ctx->elem_ctx)->base_ring = (gr_ctx_struct *) base_ring;
    ((matrix_ctx_t *) ctx->elem_ctx)->n = n;

    if (!_matrix_methods2_initialized)
    {
        gr_method_tab_init(&_matrix_methods2, matrix_methods2);
        _matrix_methods2_initialized = 1;
    }

    ctx->methods2 = &_matrix_methods2;

    ctx->debug_string = "matrix ring";
}

void
gr_ctx_clear_matrix(gr_ctx_t ctx)
{
    flint_free(ctx->elem_ctx);
}



/* todo: thread safe */
int _nmod8_methods2_initialized = 0;
gr_method_tab_t _nmod8_methods2;

gr_method_tab_input nmod8_methods2[] =
{
    {GR_METHOD_INIT,            (gr_funcptr) nmod8_init},
    {GR_METHOD_CLEAR,           (gr_funcptr) nmod8_clear},
    {GR_METHOD_SWAP,            (gr_funcptr) nmod8_swap},
    {GR_METHOD_RANDTEST,        (gr_funcptr) nmod8_randtest},
    {GR_METHOD_WRITE,           (gr_funcptr) nmod8_write},
    {GR_METHOD_ZERO,            (gr_funcptr) nmod8_zero},
    {GR_METHOD_ONE,             (gr_funcptr) nmod8_one},
    {GR_METHOD_IS_ZERO,         (gr_funcptr) nmod8_is_zero},
    {GR_METHOD_IS_ONE,          (gr_funcptr) nmod8_is_one},
    {GR_METHOD_EQUAL,           (gr_funcptr) nmod8_equal},
    {GR_METHOD_SET,             (gr_funcptr) nmod8_set},
    {GR_METHOD_SET_SI,          (gr_funcptr) nmod8_set_si},
    {GR_METHOD_NEG,             (gr_funcptr) nmod8_neg},
    {GR_METHOD_ADD,             (gr_funcptr) nmod8_add},
    {GR_METHOD_ADD_SI,          (gr_funcptr) nmod8_add_si},
    {GR_METHOD_SUB,             (gr_funcptr) nmod8_sub},
    {GR_METHOD_MUL,             (gr_funcptr) nmod8_mul},
    {GR_METHOD_MUL_SI,          (gr_funcptr) nmod8_mul_si},
    {GR_METHOD_DIV,             (gr_funcptr) nmod8_div},
    {GR_METHOD_DIV_SI,          (gr_funcptr) nmod8_div_si},
    {GR_METHOD_IS_INVERTIBLE,   (gr_funcptr) nmod8_is_invertible},
    {GR_METHOD_INV,             (gr_funcptr) nmod8_inv},
    {0,                         (gr_funcptr) NULL},
};


void
gr_ctx_init_nmod8(gr_ctx_t ctx, unsigned char n)
{
    ctx->flags = GR_COMMUTATIVE_RING;
    ctx->sizeof_elem = sizeof(nmod8_struct);
    ctx->elem_ctx = flint_malloc(sizeof(nmod_t));  /* This could be something more interesting */
    nmod_init(ctx->elem_ctx, n);

    if (!_nmod8_methods2_initialized)
    {
        gr_method_tab_init(&_nmod8_methods2, nmod8_methods2);
        _nmod8_methods2_initialized = 1;
    }

    ctx->methods2 = &_nmod8_methods2;

    ctx->debug_string = "nmod8 ring";
}

void
gr_ctx_clear_nmod8(gr_ctx_t ctx)
{
    flint_free(ctx->elem_ctx);
}
