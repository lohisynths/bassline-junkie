cmake_minimum_required(VERSION 2.8.2)

project(benchmark-download NONE)

# Download and unpack benchmark at configure time
configure_file(CMakeLists.txt.in ${CMAKE_BINARY_DIR}/benchmark-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -DCMAKE_BUILD_TYPE=Release .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/benchmark-download )
if(result)
  message(FATAL_ERROR "CMake step for benchmark failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/benchmark-download )
if(result)
  message(FATAL_ERROR "Build step for benchmark failed: ${result}")
endif()

# Add benchmark directly to our build. This defines
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)

add_subdirectory(${CMAKE_BINARY_DIR}/benchmark-src
                 ${CMAKE_BINARY_DIR}/benchmark-build
                 EXCLUDE_FROM_ALL)
