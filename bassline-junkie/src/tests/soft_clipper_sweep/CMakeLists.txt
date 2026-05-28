cmake_minimum_required(VERSION 3.12)

project(soft_clipper_sweep VERSION 0.0.0 LANGUAGES CXX)

add_executable(${PROJECT_NAME}
	../../synth/dsp/SineWave.cpp
	soft_clipper_sweep.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
	../../synth
	../../synth/utils
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11 cxx_thread_local)
