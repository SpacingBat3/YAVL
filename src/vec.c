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
#include <stdbool.h>
#include <string.h>

#include "namespace.h"

typedef struct {
  void* data;
  size_t allignment;
  size_t size;
  size_t reservd;
} NS(vec_t);

#include <YAVL/vec.h>

NS(vec_res_t) NS(vec_init)(NS(vec_t) *const vec, size_t data_align, size_t data_reserv) {
  if(vec==NULL) return NS_UPPER(VEC_RES_NULL);
  vec->size=0;
  vec->reservd=(!data_reserv)?5:data_reserv;
  vec->allignment=(!data_align)?1:data_align;
  if(vec->data) free(vec->data);
  vec->data=calloc(vec->reservd, vec->allignment);
  return vec->data == NULL ? NS_UPPER(VEC_RES_OOM) : NS_UPPER(VEC_RES_OK);
}

NS(vec_res_t) NS(vec_push)(NS(vec_t) *const vec, const void *const data) {
  if(vec==NULL) return false;
  if(vec->size >= vec->reservd) {
    vec->reservd+=5;
    void *const new = realloc(vec->data, vec->reservd*vec->allignment);
    if(!new) return false;
    vec->data = new;
  }
  if(!memcpy(vec->data+((vec->size++)*vec->allignment),
      data,vec->allignment)) {
    --vec->size;
    return false;
  }
  return true;
}

NS(vec_errorable_t) NS(vec_pop)(NS(vec_t) *const vec) {
  if(vec==NULL) return (NS(vec_errorable_t)){.status=NS_UPPER(VEC_RES_NULL),.mem=NULL};
  if(vec==NULL || vec->size == 0) return (NS(vec_errorable_t)){.status=NS_UPPER(VEC_RES_FAIL),.mem=NULL};
  return (NS(vec_errorable_t)){
    .status=NS_UPPER(VEC_RES_OK),
    .mem=vec->data+((--vec->size)*vec->allignment)
  };
}

NS(vec_res_t) NS(vec_fit)(NS(vec_t)* const vec) {
  if(vec == NULL) return NS_UPPER(VEC_RES_NULL);
  // Already alligned
  if(vec->reservd == vec->allignment) return NS_UPPER(VEC_RES_OK);
  // Resize memory allocated to memory taken
  void *const new = realloc(vec->data, vec->size*vec->allignment);
  // Realloc check for very bad allocators
  if(!new) return NS_UPPER(VEC_RES_OOM);
  vec->allignment=vec->size;
  vec->data = new;
  // We're good
  return NS_UPPER(VEC_RES_OK);
}

NS(vec_res_t) NS(vec_set)(NS(vec_t)* const vec, size_t offset, const void *const data) {
  // Simple bound checks
  if(vec == NULL) return NS_UPPER(VEC_RES_NULL);
  if(offset >= vec->size) return NS_UPPER(VEC_RES_FAIL);
  // Copy memory
  return !!memcpy(
    vec->data+(offset*vec->allignment),
    data,
    vec->allignment
  );
}

NS(vec_res_t) NS(vec_get)(NS(vec_t)* const vec, size_t offset, void *const out) {
  // Simple bound checks
  if(vec != NULL && offset >= vec->size) return false;
  // Copy memory
  return !!memcpy(
    out,
    vec->data+(offset*vec->allignment),
    vec->allignment
  );
}

NS(vec_errorable_t) NS(vec_ref)(NS(vec_t)* const vec) {
  if(vec == NULL) return (NS(vec_errorable_t)){
    .status=NS_UPPER(VEC_RES_NULL), .mem=NULL
  };
  return (NS(vec_errorable_t)) {
    .status=NS_UPPER(VEC_RES_OK), .mem=vec->data
  };
}

NS(vec_res_t) NS(vec_chkptr)(const NS(vec_t)* const vec, const void *const ptr) {
  return
    // Test boundaries
    ptr > vec->data && ptr < vec->data+(vec->allignment*vec->reservd) &&
    // Test allignment
    (((ptrdiff_t)(ptr-vec->data)) % vec->allignment) == 0;
};

NS(vec_errorable_t) NS(vec_flush)(NS(vec_t)* const vec) {
  NS(vec_res_t) status;
  switch(status = NS(vec_fit)(vec)) {
    // OOM when fitting means bad allocator
    // This is not critical.
    case NS_UPPER(VEC_RES_OOM):
    case NS_UPPER(VEC_RES_OK): {
      void* arr = vec->data;
      vec->data = NULL;
      vec->size=0;
      vec->reservd=0;
      vec->allignment=0;
      return (NS(vec_errorable_t)){.status=status, .mem=arr};
    } break;
    default:
      return (NS(vec_errorable_t)){status, .mem=NULL};
  };
}

NS(vec_res_t) NS(vec_free)(NS(vec_t)* const vec) {
  if(vec==NULL) return NS_UPPER(VEC_RES_NULL);
  vec->size=0;
  vec->reservd=0;
  vec->allignment=0;
  if(vec->data) free(vec->data);
  vec->data=NULL;
  return NS_UPPER(VEC_RES_OK);
}
