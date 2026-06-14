#include "YAVL/vec.h"
#include "prefix.h"
#include <TestRunner.h>
#include <stdlib.h>
#include <stdio.h>

it_should(convert_arrays) {
  char *const heaparr = calloc(5,sizeof(char));
  int code=0;
  if(heaparr==NULL) return 2; // OOM is not our problem
  for(char i=0;i<4;++i) heaparr[i]=i;
  yavl_vec_t vec = YAVL_VEC_T_ALLOCATOR;
  printf("0. Prepare...\n");
  test_if(yavl_vec_fromarray(&vec, heaparr, sizeof(char), 4) == YAVL_VEC_RES_OK)
    return code;
  printf("1. Arr -> Vec\n");
  // from that point, YAVL took pointer ownership
  test_if(yavl_vec_scale(&vec, 10) == YAVL_VEC_RES_OK)
    goto exit;
  printf("2. Vec[5] -> Vec[10]\n");
  test_if(yavl_vec_push(&vec, &B[0], sizeof(B)/sizeof(B[0])) == YAVL_VEC_RES_OK)
    goto exit;
  printf("3. Vec[4-10] = B\n");
  test_if(vec.len == vec.reservd
      && vec.reservd == sizeof(B)/sizeof(B[0])+4)
    goto exit;
  printf("4. ok(Vec.len,Vec.reservd)\n");
  // check data
  size_t i=0;
  for(;i<4;++i) {
    char j;
    yavl_vec_get(&vec, i, &j);
    test_if(heaparr[i]==j) printf("Bug: read: %i!=%i\n",heaparr[i],j);
    printf(" * %2zu | %2i\n",i,j);
    test_if(j==i) goto exit;
  }
  for(;i<10;++i) {
    char j;
    yavl_vec_get(&vec, i, &j);
    printf(" * %2i | %2i\n",B[i-4],j);
    test_if(j==B[i-4]) break;
  }
  printf("5. Data check\n");
  exit: yavl_vec_free(&vec);
  return code;
}
