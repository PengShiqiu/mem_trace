
cmake_minimum_required (VERSION 3.8)
project (mem_trace_test)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

add_definitions(
  -DMEM_PRINT_ENABLE
  -DMEM_TRACE_PATH="./mem_trace/"
)

# build library
add_library(mem_trace  
  mem_trace.c
  mem_trace.h)
target_link_libraries(mem_trace; -ldl)
set_target_properties(mem_trace PROPERTIES LINKER_LANGUAGE CXX)

add_executable (mem_trace_test
  main.cpp
)
target_link_libraries(mem_trace_test 
  mem_trace
)