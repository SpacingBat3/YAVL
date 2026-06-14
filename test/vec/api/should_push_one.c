#include <stdbool.h>
#include <stdio.h>
#include <YAVL/vec.h>

#include "prefix.h"
#include <TestRunner.h>

it_should(push_one) {
  yavl_vec_t vec = YAVL_VEC_T_ALLOCATOR;
  char testcase = -1;
  int code = 0;

  printf("1. Init\n");
  test_if(yavl_vec_init(&vec, sizeof(A[0]), 1) == YAVL_VEC_RES_OK)
    return code;
  printf("2. Push\n");
  test_if(yavl_vec_push(&vec, &A[0], 1) == YAVL_VEC_RES_OK)
    goto exit;
  printf("3. Get\n");
  test_if(yavl_vec_get(&vec, 0, &testcase) == YAVL_VEC_RES_OK)
    goto exit;
  printf("4. Compares\n");
  test_if(testcase == A[0]) printf(" - tetcase (%i/%i)\n",testcase,A[0]);
  test_if(vec.len == vec.reservd) printf(" - len (%zu/%zu)\n",vec.len,vec.reservd);
  is(vec.reservd == 1);

  exit: is(yavl_vec_free(&vec) == YAVL_VEC_RES_OK);
  return code;
}
