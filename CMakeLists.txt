cmake_minimum_required(VERSION 4.2)
project(YAVL
  DESCRIPTION "Yet Another Vector Library – still better than C++ impl. :P"
  VERSION 0.1.0
  LANGUAGES C
)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

find_package(Doxygen OPTIONAL_COMPONENTS doxygen)

# CMake defaults overrides
# ========================
### 1. Build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()
### 2. Output paths
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/dist/${CMAKE_BUILD_TYPE}")
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/dist/${CMAKE_BUILD_TYPE}")
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/dist/${CMAKE_BUILD_TYPE}")
endif()
set("${PROJECT_NAME}_GEN_HEADERS_OUTPUT_DIRECTORY" "${CMAKE_CURRENT_BINARY_DIR}/include")
set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/docs")

### 3. Development-centered settings
if(NOT CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # CLANGD requirements
endif()

# Options & tunables
# ==================
## 1. Namespace
set(CACHE{YAVL_C_NAMESPACE}
    TYPE STRING
    HELP "A prefix for exported C symbols in the library"
    VALUE "yavl")
option(YAVL_FAST "Sacrifice stability/safety for performance" OFF)
message(VERBOSE "YAVL: C symbol namespace: ${YAVL_C_NAMESPACE}")

## 2. List of auto-generated headers
set("${PROJECT_NAME}_GEN_HEADERS" "vec.h")

# Target list
# ===========
add_library("${PROJECT_NAME}Interface"
  INTERFACE)
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
    "${${PROJECT_NAME}_GEN_HEADERS_OUTPUT_DIRECTORY}/YAVL/"
  )
endif()

# Make shared library output be called the same as static ones.
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
      "$<BUILD_INTERFACE:${${PROJECT_NAME}_GEN_HEADERS_OUTPUT_DIRECTORY}>"
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

if(YAVL_FAST)
  target_compile_definitions("${PROJECT_NAME}Object" PRIVATE "YAVL_FAST=1")
endif()

# Header generation and inclusion to API
# ======================================
foreach(header "${${PROJECT_NAME}_GEN_HEADERS}")
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/template/include/YAVL/${header}.in"
    "${${PROJECT_NAME}_GEN_HEADERS_OUTPUT_DIRECTORY}/YAVL/${header}"
  )
  target_sources("${PROJECT_NAME}Interface" PUBLIC
    FILE_SET HEADERS
    TYPE HEADERS
    BASE_DIRS
      "${CMAKE_CURRENT_SOURCE_DIR}/include"
      "${${PROJECT_NAME}_GEN_HEADERS_OUTPUT_DIRECTORY}"
    FILES
      "${${PROJECT_NAME}_GEN_HEADERS_OUTPUT_DIRECTORY}/YAVL/${header}"
  )
endforeach()

# Arch PKGBUILD
# =============
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/template/build/PKGBUILD.in"
  "${CMAKE_CURRENT_BINARY_DIR}/meta/archlinux/PKGBUILD"
)

# Installation
# ============
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/template/build/config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/meta/cmake/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/meta/cmake/${PROJECT_NAME}ConfigVersion.cmake"
  COMPATIBILITY SameMinorVersion
)
export(PACKAGE "${PROJECT_NAME}")
install(EXPORT "${PROJECT_NAME}Targets"
    NAMESPACE "${PROJECT_NAME}::"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    COMPONENT "CMake"
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/meta/cmake/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/meta/cmake/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
  COMPONENT "CMake"
)
export(EXPORT "${PROJECT_NAME}Targets")
install(TARGETS
      "${PROJECT_NAME}"
      "${PROJECT_NAME}Interface"
      "${PROJECT_NAME}Shared"
    EXPORT "${PROJECT_NAME}Targets"
    FILE_SET HEADERS
      COMPONENT "Development"
    ARCHIVE
      COMPONENT "StaticLibrary"
    LIBRARY
      COMPONENT "SharedLibrary"
      NAMELINK_COMPONENT
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
