#include <threads.h>
#include <assert.h>

int thr_fun(void*_) {
  return 0xADD;
}

int main() {
  thrd_t thr;
  thrd_create(&thr, thr_fun, NULL);
  int var;
  thrd_join(thr, &var);
  assert(var==0xADD);
  return 0;
}