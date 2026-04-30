cmake_minimum_required(VERSION 4.2)
project(YAVL
  DESCRIPTION "Yet Another Vector Library – still better than C++ impl. :P"
  VERSION 0.1.0
  LANGUAGES C
)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

find_package(Doxygen OPTIONAL_COMPONENTS doxygen)

# In-code namespace for Library

set(CACHE{YAVL_C_NAMESPACE}
    TYPE STRING
    HELP "A prefix for exported C symbols in the library"
    VALUE "yavl"
)
message(VERBOSE "YAVL: C symbol namespace: ${YAVL_C_NAMESPACE}")

# Target list

add_library("${PROJECT_NAME}Object"
  OBJECT "src/vec.c")
add_library("${PROJECT_NAME}"
  STATIC $<TARGET_OBJECTS:${PROJECT_NAME}Object>)
add_library("${PROJECT_NAME}Shared"
  SHARED $<TARGET_OBJECTS:${PROJECT_NAME}Object>)

if(Doxygen_FOUND)
  # Doxygen generation (WIP)
  set(DOXYGEN_STRIP_FROM_PATH "${CMAKE_BINARY_DIR}")
  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/Readme.md")
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

foreach(target "${PROJECT_NAME}"
    "${PROJECT_NAME}Shared" "${PROJECT_NAME}Object")
  set_target_properties("${target}" PROPERTIES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
  )
  # Target development properties
  target_include_directories("${target}" PUBLIC
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
      "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
endforeach()

if(YAVL_C_NAMESPACE)
    string(TOUPPER "${YAVL_C_NAMESPACE}" YAVL_C_NAMESPACE_UPPER)
    if(NOT YAVL_C_NAMESPACE STREQUAL "")
        set(YAVL_C_NAMESPACE_ "${YAVL_C_NAMESPACE}_")
        set(YAVL_C_NAMESPACE_UPPER_ "${YAVL_C_NAMESPACE_UPPER}_")
    endif()
    target_compile_definitions("${PROJECT_NAME}Object"
        PRIVATE
        "YAVL_C_NAMESPACE=${YAVL_C_NAMESPACE}"
        "YAVL_C_NAMESPACE_UPPER=${YAVL_C_NAMESPACE_UPPER}"
    )
endif()

# Header generation
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/template/include/YAVL/vec.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/YAVL/vec.h"
)

# Arch PKGBUILD
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/template/build/PKGBUILD.in"
  "${CMAKE_CURRENT_BINARY_DIR}/scripts/PKGBUILD"
)

# Installation

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/template/build/config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  COMPATIBILITY SameMinorVersion
)
export(PACKAGE "${PROJECT_NAME}")
install(EXPORT "${PROJECT_NAME}Targets"
    NAMESPACE "${PROJECT_NAME}::"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    COMPONENT "CMake"
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
  COMPONENT "CMake"
)
export(EXPORT "${PROJECT_NAME}Targets")
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

if(CMAKE_MAJOR_VERSION GREATER_EQUAL 4
    AND CMAKE_MINOR_VERSION GREATER_EQUAL 3)
  install(PACKAGE_INFO "${PROJECT_NAME}"
    EXPORT "${PROJECT_NAME}Targets"
    VERSION "${PROJECT_VERSION}"
    LICENSE "GPL-3.0-or-later"
    VERSION_SCHEMA simple
    COMPONENT "CPS")
endif()
