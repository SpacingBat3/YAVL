cmake_minimum_required(VERSION 4.2)
project(YAVL
  DESCRIPTION "Yet Another Vector Library – still better than C++ impl. :P"
  VERSION 0.1.0
  LANGUAGES C
)
include(GNUInstallDirs)

# In-code namespace for Library

set(CACHE{YAVL_C_NAMESPACE}
    TYPE STRING
    HELP "A prefix for exported C symbols in the library"
    VALUE "yavl"
)
message(STATUS "C symbol namespace: ${YAVL_C_NAMESPACE}")

# Target list

add_library("${PROJECT_NAME}" STATIC "src/vec.c")

# Target install properties

set_target_properties("${PROJECT_NAME}" PROPERTIES
    PUBLIC_HEADER "${CMAKE_CURRENT_BINARY_DIR}/include/YAVL/vec.h"
    PRIVATE_HEADER "include/YAVL/_macro.h"
)

# Target development properties

target_include_directories("${PROJECT_NAME}" PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

if(YAVL_C_NAMESPACE)
    string(TOUPPER "${YAVL_C_NAMESPACE}" YAVL_C_NAMESPACE_UPPER)
    if(NOT YAVL_C_NAMESPACE STREQUAL "")
        set(YAVL_C_NAMESPACE_ "${YAVL_C_NAMESPACE}_")
        set(YAVL_C_NAMESPACE_UPPER_ "${YAVL_C_NAMESPACE_UPPER}_")
    endif()
    target_compile_definitions("${PROJECT_NAME}"
        PRIVATE
        "${YAVL_C_NAMESPACE_UPPER_}VEC_H_STRUCT"
        "YAVL_C_NAMESPACE=${YAVL_C_NAMESPACE}"
        "YAVL_C_NAMESPACE_UPPER=${YAVL_C_NAMESPACE_UPPER}"
    )
endif()

# Header generation
configure_file("include/YAVL/vec.h.in" "include/YAVL/vec.h")

# Installation

install(EXPORT "${PROJECT_NAME}Targets"
    NAMESPACE "${PROJECT_NAME}::"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)
install(TARGETS "${PROJECT_NAME}"
    EXPORT "${PROJECT_NAME}Targets"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
    PRIVATE_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
)
