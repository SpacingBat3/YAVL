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

#include "macro.h"

#include <YAVL/vec.h>

// This is extremely optimized memcpy replacement
// Made this by benchmarking.
// Inline makes difference here in regard to the
// performance, but some small wins were gained
// by limiting jumps and operations in code for
// the (assumed) most commonly executed logic.
static inline void adaptcpy(void * dest, const void * src,
    register size_t len, size_t allignment) {
  #ifndef YAVL_FAST
  if(UNLIKELY(len==0)) return;
  #endif
  if(UNLIKELY(len&1)) /* Optimized for single and non-optimizable cases. */
    while((len&1)==0&&allignment<8) len>>=1,allignment<<=1;
  map:switch (allignment) {
    #define memcpy_typed(T) \
      while(len) --len,((T*)dest)[len]=((T*)src)[len]; return
    case sizeof(uint8_t):  memcpy_typed(uint8_t);
    case sizeof(uint16_t): memcpy_typed(uint16_t);
    case sizeof(uint32_t): memcpy_typed(uint32_t);
    case sizeof(uint64_t): memcpy_typed(uint64_t);
    default: len<<=1,allignment>>=1; goto map;
    #undef memcpy_typed
  }
}

NS(vec_res_t) NS( vec_init )(NS(vec_t) *const vec, size_t data_align, size_t data_reserv) {
  #ifndef YAVL_FAST
  if(UNLIKELY(vec==NULL)) return NS_UPPER(VEC_RES_NULL);
  if(UNLIKELY(vec->data&&vec->allign)) free(vec->data);
  #endif
  vec->len=0;
  vec->allign=(!data_align)?1:data_align;
  vec->reservd=(!data_reserv)?data_align:data_reserv;
  vec->data=calloc(vec->reservd, vec->allign);
  return UNLIKELY(vec->data == NULL) ? NS_UPPER(VEC_RES_OOM) : NS_UPPER(VEC_RES_OK);
}

NS(vec_res_t) NS( vec_fromarray )(NS(vec_t) *const vec, void *const array,
      const size_t alignment, const size_t length) {
    #ifndef YAVL_FAST
    if(UNLIKELY(vec==NULL || array==NULL))
      return NS_UPPER(VEC_RES_NULL);
    #endif
    vec->len=(vec->reservd=length);
    vec->data=array;
    vec->allign=alignment;
    return NS_UPPER(VEC_RES_OK);
}

NS(vec_res_t) NS( vec_push )(NS(vec_t) *const vec, const void *const data, const size_t num_el) {
  #ifndef YAVL_FAST
  if(UNLIKELY(vec==NULL))
    return NS_UPPER(VEC_RES_NULL);
  #endif
  if(LIKELY(vec->reservd>=vec->len+num_el)) {
    alloc:adaptcpy(
      vec->data+(vec->len*vec->allign),
      data,
      num_el,
      vec->allign
    ),vec->len+=num_el;
    return NS_UPPER(VEC_RES_OK);
  } else /* UNLIKELY(vec->reservd<vec->len+num_el) */ {
    void *const new = realloc(vec->data, (vec->reservd *= num_el*2)*vec->allign);
    if(LIKELY(new!=NULL)) {
      vec->data = new;
      goto alloc;
    } else /* UNLIKELY(new==NULL) */ {
      vec->reservd /= num_el*2;
      return NS_UPPER(VEC_RES_OOM);
    }
  }
}

NS(vec_errorable_t) NS( vec_pop )(NS(vec_t) *const vec, const size_t num_el) {
  #ifndef YAVL_FAST
  if(UNLIKELY(vec==NULL || !num_el)) return (NS(vec_errorable_t)){
      .status=NS_UPPER(VEC_RES_NULL),
      .mem=NULL
  };
  #endif
  if(UNLIKELY(vec->len < num_el)) return (NS(vec_errorable_t)){
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
  if(UNLIKELY(vec == NULL))
    return NS_UPPER(VEC_RES_NULL);
  // Already alligned
  if(UNLIKELY(vec->reservd == vec->len))
    return NS_UPPER(VEC_RES_OK);
  // Resize memory allocated to memory taken
  void *const new = realloc(vec->data, (vec->reservd=vec->len)*vec->allign);
  // Realloc check for very bad allocators
  if(UNLIKELY(new == NULL)) return NS_UPPER(VEC_RES_OOM);
  vec->data = new;
  #else
  vec->data = realloc(vec->data, (vec->reservd=vec->len)*vec->allign);
  #endif
  // We're good
  return NS_UPPER(VEC_RES_OK);
}

NS(vec_res_t) NS( vec_scale )(NS(vec_t)* const vec, size_t new_reservd) {
    #ifndef YAVL_FAST
    if(UNLIKELY(vec == NULL)) return NS_UPPER(VEC_RES_NULL);
    // Already alligned
    if(UNLIKELY(vec->reservd == new_reservd)) return NS_UPPER(VEC_RES_OK);
    #endif
    void *const new = realloc(vec->data, new_reservd*vec->allign);
    if(UNLIKELY(new == NULL)) {
      return NS_UPPER(VEC_RES_OOM);
    }
    vec->data = new;
    vec->len = vec->len < new_reservd ? vec->len : new_reservd;
    vec->reservd = new_reservd;
    return NS_UPPER(VEC_RES_OK);
}

NS(vec_res_t) NS( vec_set )(NS(vec_t)* const vec, size_t offset, const void *const data) {
  #ifndef YAVL_FAST
  // Simple bound checks
  if(UNLIKELY(vec == NULL)) return NS_UPPER(VEC_RES_NULL);
  if(UNLIKELY(offset >= vec->len)) return NS_UPPER(VEC_RES_FAIL);
  #endif
  // Copy memory
  adaptcpy(
    vec->data+(offset*vec->allign),
    data,
    1,
    vec->allign
  );
  return NS_UPPER(VEC_RES_OK);
}

NS(vec_res_t) NS( vec_get )(NS(vec_t)* const vec, size_t offset, void *const out) {
  #ifndef YAVL_FAST
  // Simple bound checks // FIXME: opt oportunities
  if(UNLIKELY(vec == NULL)) return NS_UPPER(VEC_RES_NULL);
  if(UNLIKELY(offset >= vec->len)) return NS_UPPER(VEC_RES_FAIL);
  #endif
  // Copy memory
  adaptcpy(
    out,
    vec->data+(offset*vec->allign),
    1,
    vec->allign
  );
  return NS_UPPER(VEC_RES_OK);
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
  // Assume it was ensured to be fitted.
  NS(vec_res_t) status = LIKELY(vec->len==vec->reservd) ? NS_UPPER(VEC_RES_OK) : NS_UPPER(VEC_RES_OOM);
  #else
  NS(vec_res_t) status = NS(vec_fit)(vec);
  if(UNLIKELY(final_len == NULL)) status = NS_UPPER(VEC_RES_NULL);
  switch(status) {
    // OOM when fitting means bad allocator
    // This is not critical.
    case NS_UPPER(VEC_RES_OOM):
    case NS_UPPER(VEC_RES_OK): {
      #endif
      // Logical array size
      (*final_len) = vec->len;
      void* arr = vec->data;
      #ifndef YAVL_FAST
      vec->data = NULL;
      vec->len=0,
      vec->reservd=0,
      vec->allign=0;
      #endif
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
  if(UNLIKELY(vec==NULL)) return NS_UPPER(VEC_RES_NULL);
  if(LIKELY(vec->data && vec->allign)) free(vec->data);
  vec->len=0;
  vec->reservd=0;
  vec->allign=0;
  vec->data=NULL;
  #else
  free(vec->data);
  #endif
  return NS_UPPER(VEC_RES_OK);
}
