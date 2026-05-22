#ifdef __cplusplus
#include <vector>
extern "C" {
#define cpp_vector(T,...) std::vector<T __VA_OPT__(,) __VA_ARGS__>
#else
#define cpp_vector(T,...) struct vector_##T
/// Type definition
cpp_vector(char);
#endif
#include <stddef.h>
#include <stdbool.h>
// C++ API maps
cpp_vector(char) *const cpp_vec_new();
void cpp_vec_free(cpp_vector(char) *const vec);
void cpp_vec_push(cpp_vector(char) *const vec, const char data);
char cpp_vec_pop(cpp_vector(char) *const vec, const char data);
char* cpp_vec_ref(cpp_vector(char) *const vec, size_t idx);
size_t cpp_vec_get_len(cpp_vector(char) *const vec);
size_t cpp_vec_get_reservd(cpp_vector(char) *const vec);
size_t cpp_vec_get_reservd_max(cpp_vector(char) *const vec);
void cpp_vec_set_reservd(cpp_vector(char) *const vec, size_t new_reservd);
char cpp_vec_get_el(cpp_vector(char) *const vec, size_t idx);
char cpp_vec_set_el(cpp_vector(char) *const vec, size_t idx, char v);
#ifdef __cplusplus
}
#endif
