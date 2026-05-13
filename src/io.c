/*
 * io.c - I/O API based on vector for dynamic allocation
 *
 * SPDX-License: GPL-3.0-or-later
 *
 * Copyright (C) 2026 Dawid Papiewski
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this
 * program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include <stdio.h>
#include <YAVL/vec.h>
#include <YAVL/io.h>

#include "alias.h"
#include "macro.h"

vec_res_t io_fill(vec_t *const vec, FILE *const stream) {
    if(vec->allign != sizeof(char)) return VEC_RES_FAIL;
    for(
        size_t cr=1;
        cr!=0;
        cr=fread(
            /*__ptr=*/ vec->data+(vec->len*vec->allign),
            /*__size=*/ vec->allign,
            /*__n=*/ vec->reservd-vec->len,
            /*__stream=*/ stream
        )
    ) {
        vec->len += cr;
        // Autoscale
        vec_res_t res;
        if(vec->reservd <= vec->len) res = vec_scale(vec, vec->reservd*2);
        if(res != VEC_RES_OK) return res;
    }
    vec_fit(vec);
    return VEC_RES_OK;
}

vec_res_t io_readline(vec_t *const vec, FILE *const stream) {
    if(vec->allign != sizeof(char)) return VEC_RES_FAIL;
    for(
        int ch=0;
        ch!='\n'||ch==EOF;
        ch=getc(stream)
    ) {
        char c = ch; vec_res_t res;
        if((res = vec_push(vec,&c,1)) != VEC_RES_OK)
            return res;
    }
    const char trail[] = "\n";
    vec_push(vec, trail, sizeof(trail)/sizeof(trail[0]));
    return vec_fit(vec);
}
