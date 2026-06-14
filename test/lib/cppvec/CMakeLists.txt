add_library("${PROJECT_NAME}RefLib" STATIC src/lib.cpp)
add_library("${PROJECT_NAME}::RefLib" ALIAS "${PROJECT_NAME}RefLib")
target_include_directories("${PROJECT_NAME}RefLib" PUBLIC "include/")
