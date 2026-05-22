#include <stdbool.h>
#include <stddef.h>
#include <YAVL/vec.h>

#include "prefix.h"
#include <TestRunner.h>

it_should(push_many) {
  yavl_vec_t vec = YAVL_VEC_T_ALLOCATOR;
  char testcase = -1;
  int code = 0;

  test_if(yavl_vec_init(&vec, sizeof(A[0]), countof(A)) == YAVL_VEC_RES_OK)
    return code;
  test_if(yavl_vec_push(&vec, A, countof(A)) == YAVL_VEC_RES_OK)
    goto exit;
  test_if(vec.len == vec.reservd && vec.reservd == countof(A))
    goto exit;
  for(size_t i = 0; i < countof(A); ++i) {
    char tescase = -1;
    test_if(yavl_vec_get(&vec, i, &testcase) == YAVL_VEC_RES_OK)
      break;
    test_if(testcase == A[i]) {
      break;
    }
  }

  exit: is(yavl_vec_free(&vec) == YAVL_VEC_RES_OK);
  return code;
}
