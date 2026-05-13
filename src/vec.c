/*
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

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "namespace.h"

#include <YAVL/vec.h>

NS(vec_res_t) NS( vec_init )(NS(vec_t) *const vec, size_t data_align, size_t data_reserv) {
  #ifndef YAVL_FAST
  if(vec==NULL) return NS_UPPER(VEC_RES_NULL);
  if(vec->data&&vec->allign) free(vec->data);
  #endif
  vec->len=0;
  vec->allign=(!data_align)?1:data_align;
  vec->reservd=(!data_reserv)?data_align:data_reserv;
  vec->data=calloc(vec->reservd, vec->allign);
  return vec->data == NULL ? NS_UPPER(VEC_RES_OOM) : NS_UPPER(VEC_RES_OK);
}

NS(vec_res_t) NS( vec_fromarray )(NS(vec_t) *const vec, void *const array, const size_t alignment, const size_t length) {
    #ifndef YAVL_FAST
    if(vec==NULL || array==NULL) return NS_UPPER(VEC_RES_NULL);
    if(vec->data&&vec->allign) free(vec->data);
    #endif
    vec->len=(vec->reservd=length);
    vec->data=vec;
    return NS_UPPER(VEC_RES_OK);
}

NS(vec_res_t) NS( vec_push )(NS(vec_t) *const vec, const void *const data, const size_t num_el) {
  #ifndef YAVL_FAST
  if(vec==NULL) return NS_UPPER(VEC_RES_NULL);
  #endif
  if(vec->len+num_el > vec->reservd) {
    vec->reservd*=2;
    if(vec->reservd-vec->len < num_el) vec->reservd += num_el;
    void *const new = realloc(vec->data, vec->reservd*vec->allign);
    if(new) vec->data = new;
    else return NS_UPPER(VEC_RES_OOM);
  }
  if(memcpy(vec->data+((vec->len+=num_el)*vec->allign),
      data,vec->allign*num_el)) {
    return NS_UPPER(VEC_RES_OK);
  }
  --vec->len;
  return NS_UPPER(VEC_RES_FAIL);
}

NS(vec_errorable_t) NS( vec_pop )(NS(vec_t) *const vec, const size_t num_el) {
  #ifndef YAVL_FAST
  if(vec==NULL || !num_el) return (NS(vec_errorable_t)){
      .status=NS_UPPER(VEC_RES_NULL),
      .mem=NULL
  };
  #endif
  if(vec->len < num_el) return (NS(vec_errorable_t)){
      .status=NS_UPPER(VEC_RES_FAIL),
      .mem=NULL
  };
  return (NS(vec_errorable_t)){
    .status=NS_UPPER(VEC_RES_OK),
    .mem=vec->data+((vec->len-=num_el)*vec->allign)
  };
}

NS(vec_res_t) NS( vec_fit )(NS(vec_t)* const vec) {
  #ifndef YAVL_FAST
  if(vec == NULL) return NS_UPPER(VEC_RES_NULL);
  // Already alligned
  if(vec->reservd == vec->len) return NS_UPPER(VEC_RES_OK);
  // Resize memory allocated to memory taken
  void *const new = realloc(vec->data, vec->len*vec->allign);
  // Realloc check for very bad allocators
  if(!new) return NS_UPPER(VEC_RES_OOM);
  vec->data = new;
  #else
  vec->data = realloc(vec->data, vec->len*vec->allign);
  #endif
  vec->reservd=vec->len;
  // We're good
  return NS_UPPER(VEC_RES_OK);
}

NS(vec_res_t) NS( vec_scale )(NS(vec_t)* const vec, size_t new_reservd) {
    #ifndef YAVL_FAST
    if(vec == NULL) return NS_UPPER(VEC_RES_NULL);
    // Already alligned
    if(vec->reservd == new_reservd) return NS_UPPER(VEC_RES_OK);
    #endif
    void *const new = realloc(vec->data, new_reservd*vec->allign);
    if(!new) return NS_UPPER(VEC_RES_OOM);
    vec->data = new;
    vec->len = vec->len < new_reservd ? vec->len : new_reservd;
    vec->reservd = new_reservd;
    return NS_UPPER(VEC_RES_OK);
}

NS(vec_res_t) NS( vec_set )(NS(vec_t)* const vec, size_t offset, const void *const data) {
  #ifndef YAVL_FAST
  // Simple bound checks
  if(vec == NULL) return NS_UPPER(VEC_RES_NULL);
  if(offset >= vec->len) return NS_UPPER(VEC_RES_FAIL);
  #endif
  // Copy memory
  return (NS(vec_res_t) /* VEC_RES_FAIL == 1 */) !memcpy(
    vec->data+(offset*vec->allign),
    data,
    vec->allign
  );
}

NS(vec_res_t) NS( vec_get )(NS(vec_t)* const vec, size_t offset, void *const out) {
  #ifndef YAVL_FAST
  // Simple bound checks // FIXME: opt oportunities
  if(vec == NULL) return NS_UPPER(VEC_RES_NULL);
  if(offset >= vec->len) return NS_UPPER(VEC_RES_FAIL);
  #endif
  // Copy memory
  return (NS(vec_res_t) /* VEC_RES_FAIL == 1 */) !memcpy(
    out,
    vec->data+(offset*vec->allign),
    vec->allign
  );
}

bool NS( vec_chkptr )(const NS(vec_t)* const vec, const void *const ptr) {
  return
    // Test boundaries
    ptr > vec->data && ptr < vec->data+(vec->allign*vec->reservd) &&
    // Test allignment
    (((ptrdiff_t)(ptr-vec->data)) % vec->allign) == 0;
};

NS(vec_errorable_t) NS( vec_toarray )(NS(vec_t)* const vec, size_t *const final_len) {
  #ifdef YAVL_FAST
  NS(vec_res_t) status = vec->len==vec->reservd ? NS_UPPER(VEC_RES_OK) : NS_UPPER(VEC_RES_OOM);
  #else
  NS(vec_res_t) status = NS(vec_fit)(vec);
  if(final_len == NULL) status = NS_UPPER(VEC_RES_NULL);
  switch(status) {
    // OOM when fitting means bad allocator
    // This is not critical.
    case NS_UPPER(VEC_RES_OOM):
    case NS_UPPER(VEC_RES_OK): {
      #endif
      // Logical array size
      (*final_len) = vec->len;
      void* arr = vec->data;
      vec->data = NULL;
      vec->len=0;
      vec->reservd=0;
      vec->allign=0;
      return (NS(vec_errorable_t)){status, .mem=arr};
      #ifndef YAVL_FAST
    } break;
    default:
      return (NS(vec_errorable_t)){status, .mem=NULL};
  }
  #endif
}

NS(vec_res_t) NS( vec_free )(NS(vec_t)* const vec) {
  #ifndef YAVL_FAST
  if(vec==NULL) return NS_UPPER(VEC_RES_NULL);
  if(vec->data && vec->allign) free(vec->data);
  vec->len=0;
  vec->reservd=0;
  vec->allign=0;
  vec->data=NULL;
  #else
  free(vec->data);
  #endif
  return NS_UPPER(VEC_RES_OK);
}
