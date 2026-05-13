#ifndef YAVL_PRIVATE_ALIAS_H
#define YAVL_PRIVATE_ALIAS_H

#include "YAVL/vec.h"

// lowercase namespace
#ifdef YAVL_C_NAMESPACE
#define __NAMESPACE(NS,SYM) NS##_##SYM
#define _NAMESPACE(NS,SYM) __NAMESPACE(NS,SYM)
#define NS(SYM) _NAMESPACE(YAVL_C_NAMESPACE,SYM)
#define NS_TYPE(SYM) typedef NS(SYM) SYM

// type aliases
NS_TYPE(vec_t);
NS_TYPE(vec_res_t);
NS_TYPE(vec_errorable_t);

// function aliases
#define vec_init      NS(vec_init)
#define vec_fromarray NS(vec_fromarray)
#define vec_toarray   NS(vec_toarray)
#define vec_push      NS(vec_push)
#define vec_pop       NS(vec_pop)
#define vec_push      NS(vec_push)
#define vec_get       NS(vec_get)
#define vec_set       NS(vec_set)
#define vec_chkptr    NS(vec_chkptr)
#define vec_fit       NS(vec_fit)
#define vec_scale     NS(vec_scale)
#define vec_free      NS(vec_free)
#define io_fill       NS(io_fill)
#define io_readline   NS(io_readline)

#endif /* YAVL_C_NAMESPACE */

#ifdef YAVL_C_NAMESPACE_UPPER
#define __NAMESPACE_UP(NS,SYM) NS##_##SYM
#define _NAMESPACE_UP(NS,SYM) __NAMESPACE_UP(NS,SYM)
#define NS_ENUM(SYM) _NAMESPACE_UP(YAVL_C_NAMESPACE_UPPER,SYM)

#define VEC_RES_OK   NS_ENUM(VEC_RES_OK)
#define VEC_RES_FAIL NS_ENUM(VEC_RES_FAIL)
#define VEC_RES_NULL NS_ENUM(VEC_RES_NULL)
#define VEC_RES_OOM  NS_ENUM(VEC_RES_OOM)

#endif /* YAVL_C_NAMESPACE_UPPER */

#endif
