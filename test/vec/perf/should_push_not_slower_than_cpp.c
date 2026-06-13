#include "YAVL/vec.h"
#include "prefix.h"
#include <TestRunner.h>
#include <RefLib.h>

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <threads.h>

static size_t default_reservd = 0;

typedef struct {
  double res_time;
  const char *const restrict test_data;
  size_t test_data_len;
} bench_t;

static int bench_cpp(void* userdata) {
  bench_t *const data = userdata;
  struct timespec t[2] = {};
  cpp_vector(char) *cpp = NULL;
  data->res_time=0;

  for(unsigned char b=0;b<=20;++b) {
    cpp = cpp_vec_new();
    timespec_get(&t[0], TIME_UTC);
    for(size_t i=0; i<data->test_data_len; ++i)
      cpp_vec_push(cpp,data->test_data[i]);
    timespec_get(&t[1], TIME_UTC);
    cpp_vec_free(cpp);

    t[1].tv_nsec-=t[0].tv_nsec;
    t[1].tv_sec-=t[0].tv_sec;
    if(b)
      data->res_time += (((double)t[1].tv_nsec)/1e9)+(t[1].tv_sec);
  }

  return thrd_success;
}

static int bench_yavl(void* userdata) {
  bench_t *const data = userdata;
  struct timespec t[2] = {};
  yavl_vec_t yavl = YAVL_VEC_T_ALLOCATOR;
  data->res_time=0;

  for(unsigned char b=0;b<=20;++b) {
    yavl_vec_init(&yavl,sizeof(char),default_reservd);
    timespec_get(&t[0], TIME_UTC);
    for(size_t i=0; i<data->test_data_len; ++i)
      yavl_vec_push(&yavl, data->test_data+i, 1);
    timespec_get(&t[1], TIME_UTC);
    yavl_vec_free(&yavl);
    t[1].tv_nsec-=t[0].tv_nsec;
    t[1].tv_sec-=t[0].tv_sec;
    if(b)
      data->res_time += ((double)t[1].tv_nsec/1e9)+(t[1].tv_sec);
  }
  return thrd_success;
}

static int bench_inline(void* userdata) {
  bench_t *const data = userdata;
  struct timespec t[2] = {};
  data->res_time=0;

  for(unsigned char b=0;b<=20;++b) {
    char* res = malloc(1*sizeof(char));
    size_t l = 1;
    timespec_get(&t[0], TIME_UTC);
    size_t i=0;
    for(; i<data->test_data_len; ++i) {
      if(i>l) {
        l=l<<1;
        void *new = realloc(res,l*sizeof(char));
        if(new) res=new;
      }
      res[i]=data->test_data[i];
    }
    {
      void *new = realloc(res,(l=i)*sizeof(char));
      if(new) res=new;
    }
    timespec_get(&t[1], TIME_UTC);
    free(res);
    t[1].tv_nsec-=t[0].tv_nsec;
    t[1].tv_sec-=t[0].tv_sec;
    if(b)
      data->res_time += ((double)t[1].tv_nsec/1e9)+(t[1].tv_sec);
  }
  return thrd_success;
}

it_should(push_not_slower_than_cpp) {
  // RNG
  srand(time(NULL));

  // Default reservd from C++ vector
  {
    cpp_vector(char) *cpp = cpp_vec_new();
    default_reservd=cpp_vec_get_reservd(cpp);
    cpp_vec_free(cpp);
  }

  // Prepare data
  printf("Preparing data array...\n");
  size_t tdata_len = 100L*1024L*1024L;
  char *const restrict tdata = calloc(tdata_len,sizeof(tdata[0]));
  if(tdata == NULL) return 0;
  for(size_t i=0; i<tdata_len; ++i)
    tdata[i]=rand();

  bench_t bdata[3] = {
    {.test_data=tdata, .test_data_len=tdata_len},
    {.test_data=tdata, .test_data_len=tdata_len},
    {.test_data=tdata, .test_data_len=tdata_len},
  };
  thrd_t thr[3];
  thrd_start_t funs[3] = { bench_yavl,bench_cpp,bench_inline };
  char *strs[] = {"YAVL","CPP","inline"};

  char ind[3]={0,1,2};
  for (char i=2;i>0;--i) {
    char j = rand() % (i + 1);
    char temp = ind[i];
    ind[i] = ind[j];
    ind[j] = temp;
  }

  printf("Running benchmarks in their own threads...\n");
  // Random thread creation, ordered thread collection
  for(size_t i=0;i<sizeof(thr)/sizeof(thr[0]);++i){
    printf(" * Starting \"%s\"...\n",strs[ind[i]]);
    thrd_create(&thr[ind[i]], funs[ind[i]], &bdata[ind[i]]);
  }
  for(size_t i=0;i<sizeof(thr)/sizeof(thr[0]);++i){
    thrd_join(thr[i], NULL);
    printf(" * \"%s\" finished! \n",strs[i]);
  }

  printf(
    "YAVL Score (1):    %lf\n"
    " C++ Score (1):    %lf\n"
    #if COMPARE_MULTIPUSH && TODO
    "YAVL Score (many): %lf\n"
    #endif
    , (bdata[2].res_time/bdata[0].res_time)*100
    , (bdata[2].res_time/bdata[1].res_time)*100
    #if COMPARE_MULTIPUSH && TODO
    , (bdata[2].res_time/bdata[3].res_time)*100
    #endif
  );
  return bdata[0].res_time>bdata[1].res_time;
}
