#ifdef PREFIX

// Macro backups
#pragma push_macro("vec_api")
#pragma push_macro("vec_perf")

// Map names to numbers
#define vec_api  1L
#define vec_perf 2L

// Compare macro to macro by name
#if PREFIX == vec_api
#define PRIVATE_DEP_should(T,K) vec_api##K##T
#elif PREFIX == vec_perf
#define PRIVATE_DEP_should(T,K) vec_perf##K##T
#else /* PREFIX == vec_api */
#error Unhandled PREFIX case
#endif /* PREFIX == vec_api */

// Macro restore
#undef vec_perf
#undef vec_api
#pragma pop_macro("vec_perf")
#pragma pop_macro("vec_api")

// Static macros
#define it_should(T) int PRIVATE_DEP_should(T,_should_)(int argc,char** argv)
#define it_should_not(T) int PRIVATE_DEP_should(T,_should_not_)(int argc,char** argv)
#define _tostr(...) #__VA_ARGS__
#define tostr(...) _tostr(__VA_ARGS__)
#define todo(...) { _Pragma(_tostr(message("Unimplemented test case: " #__VA_ARGS__))) return 2; }
#define it_todo(T) int PRIVATE_DEP_should(T,_should_)(int argc,char** argv) todo
#undef PREFIX
#endif
