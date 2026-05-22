#include "YAVL/vec.h"
#include "prefix.h"
#include <TestRunner.h>
#include <RefLib.h>

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

it_should(push_not_slower_than_cpp) {
  struct timespec t[2] = {};
  double time[4] = {0.,0.,0.};

  // RNG
  timespec_get(&t[0], CLOCK_REALTIME);
  srand(t[0].tv_nsec);

  // Prepare data
  printf("Preparing CPP...\n");
  cpp_vector(char) *cpp = cpp_vec_new();
  const size_t default_reservd=cpp_vec_get_reservd(cpp);

  printf("Preparing YAVL...\n");
  yavl_vec_t yavl = YAVL_VEC_T_ALLOCATOR;
  cpp_vec_free(cpp);

  printf("Preparing data array...\n");
  size_t tdata_len = 100L*1024L*1024L;
  char *const restrict tdata = calloc(tdata_len,sizeof(tdata[0]));
  if(tdata == NULL) return 0;
  for(size_t i=0; i<tdata_len; ++i)
    tdata[i]=rand();

  printf("Benchmarking CPP...\n");
  for(unsigned char b=1;b<=20;++b) {
    cpp = cpp_vec_new();
    timespec_get(&t[0], TIME_UTC);
    for(size_t i=0; i<tdata_len; ++i)
      cpp_vec_push(cpp,tdata[i]);
    timespec_get(&t[1], TIME_UTC);
    cpp_vec_free(cpp);

    t[1].tv_nsec-=t[0].tv_nsec;
    t[1].tv_sec-=t[0].tv_sec;
    time[0] += (((double)t[1].tv_nsec)/1e9)+(t[1].tv_sec);
  }
  printf("Benchmarking YAVL...\n");
  for(unsigned char b=1;b<=20;++b) {
    yavl_vec_init(&yavl,sizeof(char),default_reservd);
    timespec_get(&t[0], TIME_UTC);
    for(size_t i=0; i<tdata_len; ++i)
      yavl_vec_push(&yavl, tdata+i, 1);
    timespec_get(&t[1], TIME_UTC);
    yavl_vec_free(&yavl);
    t[1].tv_nsec-=t[0].tv_nsec;
    t[1].tv_sec-=t[0].tv_sec;
    time[1] += ((double)t[1].tv_nsec/1e9)+(t[1].tv_sec);
  }

  #if COMPARE_MULTIPUSH
  printf("Benchmarking YAVL (pushing entire array)...\n");
  for(unsigned char b=1;b<=20;++b) {
    yavl_vec_init(&yavl,sizeof(char),default_reservd);
    timespec_get(&t[0], TIME_UTC);
    yavl_vec_push(&yavl, tdata, tdata_len);
    timespec_get(&t[1], TIME_UTC);
    yavl_vec_free(&yavl);
    t[1].tv_nsec-=t[0].tv_nsec;
    t[1].tv_sec-=t[0].tv_sec;
    time[2] += ((double)t[1].tv_nsec/1e9)+(t[1].tv_sec);
  }
  #endif

  printf("Benchmarking inline C array...\n");
  for(unsigned char b=1;b<=20;++b) {
    char* res = malloc(1*sizeof(char));
    size_t l = 1;
    timespec_get(&t[0], TIME_UTC);
    size_t i=0;
    for(; i<tdata_len; ++i) {
      if(i>l) {
        l=l<<1;
        void *new = realloc(res,l*sizeof(char));
        if(new) res=new;
      }
      res[i]=tdata[i];
    }
    {
      void *new = realloc(res,(l=i)*sizeof(char));
      if(new) res=new;
    }
    timespec_get(&t[1], TIME_UTC);
    free(res);
    t[1].tv_nsec-=t[0].tv_nsec;
    t[1].tv_sec-=t[0].tv_sec;
    time[3] += ((double)t[1].tv_nsec/1e9)+(t[1].tv_sec);
  }

  #if COMPARE_RESIZE
  printf(" * Test CPP resize\n"); {
    size_t c=0;
    cpp = cpp_vec_new();
    size_t reserv_old=0;
    for(size_t i=0; i<tdata_len; ++i) {
      cpp_vec_push(cpp,tdata[i]);
      size_t reserv = cpp_vec_get_reservd(cpp);
      if(reserv != reserv_old)
        printf("Reserved (%zu): %zu\n",++c,reserv_old=reserv);
    }
  }

  printf(" * Test YAVL resize\n"); {
    size_t c=0;
    yavl_vec_init(&yavl,sizeof(char),default_reservd);
    size_t reserv_old=0;
    for(size_t i=0; i<tdata_len; ++i) {
      yavl_vec_push(&yavl,tdata+i,1);
      if(yavl.reservd != reserv_old)
        printf("Reserved (%zu): %zu\n",++c,reserv_old=yavl.reservd);
    }
  }
  #endif

  printf(
    "YAVL Score (1):    %lf\n"
    " C++ Score (1):    %lf\n"
    #if COMPARE_MULTIPUSH
    "YAVL Score (many): %lf\n"
    #endif
    , (time[3]/time[1])*100
    , (time[3]/time[0])*100
    #if COMPARE_MULTIPUSH
    , (time[3]/time[2])*100
    #endif
  );
  return time[1]>time[0];
}
