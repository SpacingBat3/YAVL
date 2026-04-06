cmake_minimum_required(VERSION 4.2)
project(YAVL
  DESCRIPTION "Yet Another Vector Library – still better than C++ impl. :P"
  VERSION 0.1.0
  LANGUAGES C
)
include(GNUInstallDirs)
find_package(Doxygen OPTIONAL_COMPONENTS doxygen)

# In-code namespace for Library

set(CACHE{YAVL_C_NAMESPACE}
    TYPE STRING
    HELP "A prefix for exported C symbols in the library"
    VALUE "yavl"
)
message(STATUS "C symbol namespace: ${YAVL_C_NAMESPACE}")

# Target list

add_library("${PROJECT_NAME}Object" OBJECT "src/vec.c")
add_library("${PROJECT_NAME}"       STATIC $<TARGET_OBJECTS:${PROJECT_NAME}Object>)
add_library("${PROJECT_NAME}Shared" SHARED $<TARGET_OBJECTS:${PROJECT_NAME}Object>)

if(Doxygen_FOUND)
  # Doxygen generation (WIP)
  set(DOXYGEN_STRIP_FROM_PATH "${CMAKE_BINARY_DIR}")
  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_CURRENT_SOURCE_DIR}/docs/Readme.md")
  doxygen_add_docs("${PROJECT_NAME}Docs"
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/Readme.md"
    "${CMAKE_CURRENT_BINARY_DIR}/include/YAVL/vec.h"
  )
endif()

# Target install properties

set_target_properties("${PROJECT_NAME}" PROPERTIES
    PUBLIC_HEADER "${CMAKE_CURRENT_BINARY_DIR}/include/YAVL/vec.h"
)

set_target_properties("${PROJECT_NAME}Shared" PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}"
)

foreach(target "${PROJECT_NAME}" "${PROJECT_NAME}Shared")
  set_target_properties("${target}" PROPERTIES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
  )
endforeach()

# Target development properties

target_include_directories("${PROJECT_NAME}Object" PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

if(YAVL_C_NAMESPACE)
    string(TOUPPER "${YAVL_C_NAMESPACE}" YAVL_C_NAMESPACE_UPPER)
    if(NOT YAVL_C_NAMESPACE STREQUAL "")
        set(YAVL_C_NAMESPACE_ "${YAVL_C_NAMESPACE}_")
        set(YAVL_C_NAMESPACE_UPPER_ "${YAVL_C_NAMESPACE_UPPER}_")
    endif()
    target_compile_definitions("${PROJECT_NAME}Object"
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
    COMPONENT "CMake"
)
install(TARGETS
      "${PROJECT_NAME}"
      "${PROJECT_NAME}Shared"
    EXPORT "${PROJECT_NAME}Targets"
    ARCHIVE
      COMPONENT "StaticLibrary"
    LIBRARY
      COMPONENT "SharedLibrary"
      NAMELINK_COMPONENT
    PUBLIC_HEADER
      DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
      COMPONENT "Development"
    PRIVATE_HEADER
      DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
      COMPONENT "Development"
)

if(Doxygen_FOUND)
  install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/html"
    DESTINATION "${CMAKE_INSTALL_DOCDIR}"
    COMPONENT "Docs"
    EXCLUDE_FROM_ALL
  )
endif()

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/docs/Readme.md"
  DESTINATION "${CMAKE_INSTALL_DOCDIR}"
  COMPONENT "Docs"
  EXCLUDE_FROM_ALL
)
