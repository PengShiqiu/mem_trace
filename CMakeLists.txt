
cmake_minimum_required (VERSION 3.8)
project (mem_trace_test)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

add_definitions(
  -DMEM_PRINT_ENABLE
  -DMEM_TRACE_PATH="./mem/"
)

add_executable (mem_trace_test
  main.cpp
  mem_trace.c
  mem_trace.h
)

target_link_libraries(mem_trace_test 
  -ldl
)

