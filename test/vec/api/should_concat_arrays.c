#include <stdbool.h>
#include <stddef.h>
#include <YAVL/vec.h>

#include "prefix.h"
#include <TestRunner.h>
#include <stdio.h>

it_should(concat_arrays) {
  yavl_vec_t vec = YAVL_VEC_T_ALLOCATOR;
  struct Array {
    const size_t len;
    char *const A;
  } arrs[] = {
    (struct Array){countof(A),&A[0]},
    (struct Array){countof(B),&B[0]}
  };
  unsigned char arrInd[][countof(arrs)] = {
    {0,1},
    {1,0}
  };
  size_t total = 0;
  for(size_t i=0; i<countof(arrs); ++i) total+=arrs[i].len;

  int code = 0;
  for(size_t i=0; i<countof(arrInd); ++i) {
    yavl_vec_init(
      /*vec=*/&vec,
      /*data_align=*/sizeof(char),
      /*data_reserv=*/total
    );
    printf("0. Init\n");
    for(size_t j=0; j<countof(arrInd[i]);++j) {
      test_if(yavl_vec_push(&vec, arrs[arrInd[i][j]].A, arrs[arrInd[i][j]].len)
          == YAVL_VEC_RES_OK)
        goto exit;
    }
    printf("1. Push arrays\n");
    for(size_t j=0,ci=0; j<countof(arrInd[i]);++j) for(size_t c=0; c<arrs[arrInd[i][j]].len;++c,++ci) {
      char testcase = -1;
      test_if(yavl_vec_get(&vec,ci,&testcase) == YAVL_VEC_RES_OK)
        goto exit;
      printf("%zu: %i | %i (%u)\n",ci,testcase,arrs[arrInd[i][j]].A[c],arrInd[i][j]);
      //test_if(testcase == arrs[arrInd[i][j]].A[c]) {
      //  raise(SIGSTOP);
      //  goto exit;
      //}
    }
    printf("2. Validate data\n");
    test_if(vec.len == vec.reservd && vec.reservd == total)
      break;
    printf("3. Validate reservd\n");
    test_if(yavl_vec_free(&vec) == YAVL_VEC_RES_OK)
      break;
    printf("4. After free\n");
  }
  if(false) exit: is(yavl_vec_free(&vec) == YAVL_VEC_RES_OK);
  return code;
}
