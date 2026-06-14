#include <RefLib.h>

// Bindings for C++ std::vec to C.

extern "C" {
    cpp_vector(char) *const cpp_vec_new() {
        return new cpp_vector(char);
    }
    void cpp_vec_free(cpp_vector(char) *const vec) {
        delete vec;
    }
    void cpp_vec_push(cpp_vector(char) *const vec, const char data) {
        vec->push_back(data);
    }
    char cpp_vec_pop(cpp_vector(char) *const vec, const char data) {
        char last = vec->back();
        vec->pop_back();
        return last;
    }
    char* cpp_vec_ref(cpp_vector(char) *const vec, size_t idx) {
        return vec->data()+idx;
    }
    size_t cpp_vec_get_len(cpp_vector(char) *const vec) {
        return vec->size();
    }
    size_t cpp_vec_get_reservd(cpp_vector(char) *const vec) {
        return vec->capacity();
    }
    size_t cpp_vec_get_reservd_max(cpp_vector(char) *const vec) {
        return vec->max_size();
    }
    void cpp_vec_set_reservd(cpp_vector(char) *const vec, size_t new_reservd) {
        vec->reserve(new_reservd);
    }
    char cpp_vec_get_el(cpp_vector(char) *const vec, size_t idx) {
        return (*vec)[idx];
    }
    char cpp_vec_set_el(cpp_vector(char) *const vec, size_t idx, char v) {
        return (*vec)[idx] = v;
    }
}
