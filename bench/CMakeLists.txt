add_executable("${CMAKE_PROJECT_NAME}-benchmark" bench.cpp)
target_link_libraries("${CMAKE_PROJECT_NAME}-benchmark" PUBLIC libant)
