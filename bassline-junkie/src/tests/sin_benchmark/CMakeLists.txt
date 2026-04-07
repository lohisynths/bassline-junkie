cmake_minimum_required(VERSION 3.5)

project(sin_benchmark VERSION 0.0.0 LANGUAGES CXX)
find_package(ALSA REQUIRED)
if(NOT ALSA_FOUND )
    message(MESSAGE_FATAL "The process is not interrupted!")
endif()

# Create the executable
add_executable(${PROJECT_NAME}
	../../synth/dsp/PolyBLEPOscillator/Oscillator.cpp
	../../synth/dsp/PolyBLEPOscillator/PolyBLEPOscillator.cpp
	sin_bench.cpp
)


target_link_libraries(${PROJECT_NAME}
    asound
    pthread
    benchmark
)

target_include_directories(${PROJECT_NAME} PRIVATE
	../../synth
	../../synth/utils
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11 cxx_thread_local)
