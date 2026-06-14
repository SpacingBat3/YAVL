cmake_minimum_required(VERSION 4.2)
set(OLD_PROJECT_NAME "${PROJECT_NAME}")
project("${OLD_PROJECT_NAME}Tests" C CXX)

# Collect tests
file(GLOB_RECURSE "${OLD_PROJECT_NAME}_TESTS"
  LIST_DIRECTORIES false
  CONFIGURE_DEPENDS
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  vec/**/*.c
)

list(JOIN "${OLD_PROJECT_NAME}_TESTS" " " testrunner_str)
create_test_sourcelist(
  "${OLD_PROJECT_NAME}_TEST_SRC"
  TestRunner.c
  ${${OLD_PROJECT_NAME}_TESTS}
)

# Additional libraries
add_subdirectory("lib/")

# Test runner
add_executable("${OLD_PROJECT_NAME}TestRunner" TestRunner.c ${${OLD_PROJECT_NAME}_TEST_SRC})
add_executable("${OLD_PROJECT_NAME}::TestRunner" ALIAS "${OLD_PROJECT_NAME}TestRunner")
add_executable("${PROJECT_NAME}::Runner" ALIAS "${OLD_PROJECT_NAME}TestRunner")
target_link_libraries("${OLD_PROJECT_NAME}TestRunner" PRIVATE "${OLD_PROJECT_NAME}" "${PROJECT_NAME}RefLib")
target_include_directories("${OLD_PROJECT_NAME}TestRunner" PRIVATE "include/")
set_target_properties("${OLD_PROJECT_NAME}TestRunner" PROPERTIES
  CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test")

# Check multithreading
if(NOT DEFINED TRY_STDC_THREADS)
  message(CHECK_START "Test if <threads.h> is compiling")
  try_compile(TRY_STDC_THREADS
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/compiler/stdc_threads.c"
  )
  if(TRY_STDC_THREADS)
    message(CHECK_PASS "Success")
  else()
    message(CHECK_FAIL "Failure")
  endif()
endif()

if(TRY_STDC_THREADS)
  set(THREAD_IMPL "C")
  set(THREAD_IMPL_ENUM 1)
else(TRY_STDC_THREADS)
  #POSIX
  set(THREADS_PREFER_PTHREAD_FLAG TRUE)
  find_package(Threads)
  if(CMAKE_USE_PTHREADS_INIT AND NOT DEFINED TRY_POSIX_THREADS)
    try_compile(TRY_POSIX_THREADS
      SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/compiler/stdc_threads.c"
      LINK_LIBRARIES Threads::Threads
    )
  endif(CMAKE_USE_PTHREADS_INIT AND NOT DEFINED TRY_POSIX_THREADS)
  if(TRY_POSIX_THREADS)
    set(THREAD_IMPL "POSIX")
    set(THREAD_IMPL_ENUM 2)
    target_link_libraries("${OLD_PROJECT_NAME}TestRunner" PRIVATE Threads::Threads)
  else(TRY_POSIX_THREADS)
    set(THREAD_IMPL "<none>")
    set(THREAD_IMPL_ENUM 0)
  endif(TRY_POSIX_THREADS)
endif(TRY_STDC_THREADS)

message(STATUS "Threads implementation: ${THREAD_IMPL}")
target_compile_definitions("${OLD_PROJECT_NAME}TestRunner"
  PRIVATE "DETECTED_THREAD_IMPL=${THREAD_IMPL_ENUM}")

foreach(test IN LISTS "${OLD_PROJECT_NAME}_TESTS")
  get_filename_component(TName "${test}" NAME_WE)
  get_filename_component(TPath "${test}" DIRECTORY)
  string(REPLACE "/" "->" TNamespace "${TPath}")
  string(REPLACE "/" ";" TLabels "${TPath}")
  string(REGEX MATCH "^should(_not)?" TKind "${TName}")
  add_test(
    NAME "${TNamespace}.${TName}"
    COMMAND "${OLD_PROJECT_NAME}TestRunner" "${TPath}/${TName}"
  )
  if(TKind STREQUAL "should_not")
    set_property(
      TEST
        "${TNamespace}.${TName}"
      PROPERTY
        WILL_FAIL true
    )
  endif()
  set_property(
    TEST
      "${TNamespace}.${TName}"
    PROPERTY
      SKIP_RETURN_CODE 2
  )
  set_property(
    TEST
      "${TNamespace}.${TName}"
    PROPERTY
      LABELS ${TLabels}
  )
endforeach()
