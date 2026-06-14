#include <pthread.h>
#include <assert.h>

void* thr_fun(void* pass) {
  return pass;
}

int main() {
  pthread_t thr;
  pthread_create(&thr, NULL, thr_fun, (void*) 0xADD);
  void* var=NULL;
  pthread_join(thr, &var);
  assert(var==0xADD);
  return 0;
}