cmake_minimum_required (VERSION 3.5)

option(BASSLINE_JUNKIE_TESTING_ENABLED "Enable unit tests and benchmarks")

add_subdirectory(${PROJECT_SOURCE_DIR}/stk)
add_subdirectory(${PROJECT_SOURCE_DIR}/bassline-junkie)

if(BASSLINE_JUNKIE_TESTING_ENABLED)
	add_subdirectory(cmake/gtest)
	add_subdirectory(cmake/benchmark)
endif()

