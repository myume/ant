cmake_minimum_required(VERSION 3.30)

project(
  ant
  DESCRIPTION "A Code Annotation Tool"
  VERSION 0.1.0
)

add_compile_definitions(ANT_VERSION="${CMAKE_PROJECT_VERSION}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BENCHMARK "Compile for Benchmarking" OFF)

add_library(libant STATIC 
  src/annotator.cpp
  src/annotation.cpp
)
target_include_directories(libant PUBLIC include)

if(BENCHMARK)
  target_compile_definitions(libant PRIVATE BENCHMARK_MODE)
  target_compile_options(libant PRIVATE -O3)
endif()

add_subdirectory(src)

add_subdirectory(bench)
