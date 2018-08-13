/* Copyright (C) 2001-2018 Artifex Software, Inc.
   All Rights Reserved.

   This software is provided AS-IS with no warranty, either express or
   implied.

   This software is distributed under license and may not be copied,
   modified or distributed except as expressly authorized under the terms
   of the license contained in the file LICENSE in this distribution.

   Refer to licensing information at http://www.artifex.com or contact
   Artifex Software, Inc.,  1305 Grant Avenue - Suite 200, Novato,
   CA 94945, U.S.A., +1(415)492-9861, for further information.
*/

/* Path operations for the PDF interpreter */

#include "pdf_int.h"
#include "pdf_path.h"
#include "pdf_stack.h"
#include "gstypes.h"

int pdf_moveto (pdf_context *ctx)
{
    pdf_num *n1, *n2;
    int code;
    double x, y;

    if (ctx->stack_top - ctx->stack_bot < 2) {
        if (ctx->pdfstoponerror)
            return_error(gs_error_stackunderflow);
        else {
            pdf_clearstack(ctx);
            return 0;
        }
    }

    n1 = (pdf_num *)ctx->stack_top[-1];
    n2 = (pdf_num *)ctx->stack_top[-2];
    if (n1->type == PDF_INT){
        y = (double)n1->value.i;
    } else{
        if (n1->type == PDF_REAL) {
            y = n1->value.d;
        } else {
            if (ctx->pdfstoponerror)
                return_error(gs_error_typecheck);
            pdf_pop(ctx, 2);
            return 0;
        }
    }
    if (n2->type == PDF_INT){
        x = (double)n2->value.i;
    } else{
        if (n2->type == PDF_REAL) {
            x = n2->value.d;
        } else {
            if (ctx->pdfstoponerror)
                return_error(gs_error_typecheck);
            pdf_pop(ctx, 2);
            return 0;
        }
    }
    
    code = gs_moveto(ctx->pgs, x, y);
    if (code == 0)
        pdf_pop(ctx, 2);
    if(code < 0 && ctx->pdfstoponerror)
        return code;
    else
        return 0;
}

int pdf_lineto (pdf_context *ctx)
{
    pdf_num *n1, *n2;
    int code;
    double x, y;

    if (ctx->stack_top - ctx->stack_bot < 2) {
        if (ctx->pdfstoponerror)
            return_error(gs_error_stackunderflow);
        else {
            pdf_clearstack(ctx);
            return 0;
        }
    }

    n1 = (pdf_num *)ctx->stack_top[-1];
    n2 = (pdf_num *)ctx->stack_top[-2];
    if (n1->type == PDF_INT){
        y = (double)n1->value.i;
    } else{
        if (n1->type == PDF_REAL) {
            y = n1->value.d;
        } else {
            if (ctx->pdfstoponerror)
                return_error(gs_error_typecheck);
            pdf_pop(ctx, 2);
            return 0;
        }
    }
    if (n2->type == PDF_INT){
        x = (double)n2->value.i;
    } else{
        if (n2->type == PDF_REAL) {
            x = n2->value.d;
        } else {
            if (ctx->pdfstoponerror)
                return_error(gs_error_typecheck);
            pdf_pop(ctx, 2);
            return 0;
        }
    }
    
    code = gs_lineto(ctx->pgs, x, y);
    if (code == 0)
        pdf_pop(ctx, 2);
    if(code < 0 && ctx->pdfstoponerror)
        return code;
    else
        return 0;
}

int pdf_fill(pdf_context *ctx)
{
    int code;

    gs_swapcolors(ctx->pgs);
    code = gs_fill(ctx->pgs);
    gs_swapcolors(ctx->pgs);
    if (code == 0)
        pdf_pop(ctx, 2);
    if(code < 0 && ctx->pdfstoponerror)
        return code;
    else
        return 0;
}

int pdf_eofill(pdf_context *ctx)
{
    int code;

    gs_swapcolors(ctx->pgs);
    code = gs_eofill(ctx->pgs);
    gs_swapcolors(ctx->pgs);
    if (code == 0)
        pdf_pop(ctx, 2);
    if(code < 0 && ctx->pdfstoponerror)
        return code;
    else
        return 0;
}

int pdf_stroke(pdf_context *ctx)
{
    int code = gs_stroke(ctx->pgs);

    if(code < 0 && ctx->pdfstoponerror)
        return code;
    else
        return 0;
}

int pdf_closepath_stroke(pdf_context *ctx)
{
    int code;
    code = gs_closepath(ctx->pgs);
    if (code == 0) {
        code = gs_stroke(ctx->pgs);
    }
    if(code < 0 && ctx->pdfstoponerror)
        return code;
    else
        return 0;
}

int pdf_curveto(pdf_context *ctx)
{
    int i, code;
    pdf_num *num;
    double Values[6];

    if (ctx->stack_top - ctx->stack_bot < 6) {
        if (ctx->pdfstoponerror)
            return_error(gs_error_stackunderflow);
        else {
            pdf_clearstack(ctx);
            return 0;
        }
    }

    for (i=0;i < 6;i++){
        num = (pdf_num *)ctx->stack_top[i - 6];
        if (num->type != PDF_INT) {
            if(num->type != PDF_REAL) {
                if (ctx->pdfstoponerror)
                    return_error(gs_error_typecheck);
                pdf_pop(ctx, 6);
                return 0;
            }
            else
                Values[i] = num->value.d;
        } else {
            Values[i] = (double)num->value.i;
        }
    }
    code = gs_curveto(ctx->pgs, Values[0], Values[1], Values[2], Values[3], Values[4], Values[5]);
    if (code == 0)
        pdf_pop(ctx, 6);
    if(code < 0 && ctx->pdfstoponerror)
        return code;
    else
        return 0;
}

int pdf_v_curveto(pdf_context *ctx)
{
    int i, code;
    pdf_num *num;
    double Values[4];
    gs_point pt;

    if (ctx->stack_top - ctx->stack_bot < 4) {
        if (ctx->pdfstoponerror)
            return_error(gs_error_stackunderflow);
        else {
            pdf_clearstack(ctx);
            return 0;
        }
    }

    for (i=0;i < 4;i++){
        num = (pdf_num *)ctx->stack_top[i - 4];
        if (num->type != PDF_INT) {
            if(num->type != PDF_REAL) {
                if (ctx->pdfstoponerror)
                    return_error(gs_error_typecheck);
                pdf_pop(ctx, 4);
                return 0;
            }
            else
                Values[i] = num->value.d;
        } else {
            Values[i] = (double)num->value.i;
        }
    }
    code = gs_currentpoint(ctx->pgs, &pt);
    if (code < 0) {
        if (ctx->pdfstoponerror)
            return code;
        else
            pdf_pop(ctx, 4);
        return 0;
    }

    code = gs_curveto(ctx->pgs, pt.x, pt.y, Values[0], Values[1], Values[2], Values[3]);
    if (code == 0)
        pdf_pop(ctx, 4);
    if(code < 0 && ctx->pdfstoponerror)
        return code;
    else
        return 0;
}

int pdf_y_curveto(pdf_context *ctx)
{
    int i, code;
    pdf_num *num;
    double Values[4];

    if (ctx->stack_top - ctx->stack_bot < 4) {
        if (ctx->pdfstoponerror)
            return_error(gs_error_stackunderflow);
        else {
            pdf_clearstack(ctx);
            return 0;
        }
    }

    for (i=0;i < 4;i++){
        num = (pdf_num *)ctx->stack_top[i - 4];
        if (num->type != PDF_INT) {
            if(num->type != PDF_REAL) {
                if (ctx->pdfstoponerror)
                    return_error(gs_error_typecheck);
                pdf_pop(ctx, 4);
                return 0;
            }
            else
                Values[i] = num->value.d;
        } else {
            Values[i] = (double)num->value.i;
        }
    }
    code = gs_curveto(ctx->pgs, Values[0], Values[1], Values[2], Values[3], Values[2], Values[3]);
    if (code == 0)
        pdf_pop(ctx, 4);
    if(code < 0 && ctx->pdfstoponerror)
        return code;
    else
        return 0;
    return code;
}

int pdf_closepath(pdf_context *ctx)
{
    int code = gs_closepath(ctx->pgs);

    if(code < 0 && ctx->pdfstoponerror)
        return code;
    else
        return 0;
}

int pdf_newpath(pdf_context *ctx)
{
    int code = gs_newpath(ctx->pgs);

    if(code < 0 && ctx->pdfstoponerror)
        return code;
    else
        return 0;
}

int pdf_b(pdf_context *ctx)
{
    int code;

    code = gs_closepath(ctx->pgs);
    if (code == 0) {
        code = gs_gsave(ctx->pgs);
        if (code > 0) {
            code = gs_fill(ctx->pgs);
            if (code == 0) {
                code = gs_grestore(ctx->pgs);
                if (code == 0)
                    code = gs_stroke(ctx->pgs);
            }
        }
    }
    if(code < 0 && ctx->pdfstoponerror)
        return code;
    else
        return 0;
}

int pdf_b_star(pdf_context *ctx)
{
    int code;

    code = gs_closepath(ctx->pgs);
    if (code == 0) {
        code = gs_gsave(ctx->pgs);
        if (code > 0) {
            code = gs_eofill(ctx->pgs);
            if (code == 0) {
                code = gs_grestore(ctx->pgs);
                if (code == 0)
                    code = gs_stroke(ctx->pgs);
            }
        }
    }
    if(code < 0 && ctx->pdfstoponerror)
        return code;
    else
        return 0;
}

int pdf_B(pdf_context *ctx)
{
    int code;

    code = gs_gsave(ctx->pgs);
    if (code > 0) {
        code = gs_fill(ctx->pgs);
        if (code == 0) {
            code = gs_grestore(ctx->pgs);
            if (code == 0)
                code = gs_stroke(ctx->pgs);
        }
    }
    if(code < 0 && ctx->pdfstoponerror)
        return code;
    else
        return 0;
}

int pdf_B_star(pdf_context *ctx)
{
    int code;

    code = gs_gsave(ctx->pgs);
    if (code > 0) {
        code = gs_eofill(ctx->pgs);
        if (code == 0) {
            code = gs_grestore(ctx->pgs);
            if (code == 0)
                code = gs_stroke(ctx->pgs);
        }
    }
    return code;
}

int pdf_clip(pdf_context *ctx)
{
    int code = gs_clip(ctx->pgs);

    if(code < 0 && ctx->pdfstoponerror)
        return code;
    else
        return 0;
}

int pdf_eoclip(pdf_context *ctx)
{
    int code = gs_eoclip(ctx->pgs);

    if(code < 0 && ctx->pdfstoponerror)
        return code;
    else
        return 0;
}

int pdf_rectpath(pdf_context *ctx)
{
    int i, code;
    pdf_num *num;
    double Values[4];

    if (ctx->stack_top - ctx->stack_bot < 4) {
        if (ctx->pdfstoponerror)
            return_error(gs_error_stackunderflow);
        else {
            pdf_clearstack(ctx);
            return 0;
        }
    }

    for (i=0;i < 4;i++){
        num = (pdf_num *)ctx->stack_top[i - 4];
        if (num->type != PDF_INT) {
            if(num->type != PDF_REAL) {
                if(ctx->pdfstoponerror)
                    return_error(gs_error_typecheck);
                pdf_pop(ctx, 4);
                return 0;
            }
            else
                Values[i] = num->value.d;
        } else {
            Values[i] = (double)num->value.i;
        }
    }
    code = gs_moveto(ctx->pgs, Values[0], Values[1]);
    if (code == 0) {
        code = gs_lineto(ctx->pgs, Values[0], Values[1] + Values[3]);
        if (code == 0){
            code = gs_lineto(ctx->pgs, Values[0] + Values[2], Values[1] + Values[3]);
            if (code == 0) {
                code = gs_lineto(ctx->pgs, Values[0] + Values[2], Values[1]);
                if (code == 0){
                    code = gs_closepath(ctx->pgs);
                }
            }
        }
    }
    if (code == 0)
        pdf_pop(ctx, 4);
    if(code < 0 && ctx->pdfstoponerror)
        return code;
    else
        return 0;
}
