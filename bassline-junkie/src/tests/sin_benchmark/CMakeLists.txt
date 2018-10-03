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


if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES armv7l)
    set(ADITIONAL_LIBS jack)
endif()

target_link_libraries(${PROJECT_NAME}
    lohi::stk
    asound
    ${ADITIONAL_LIBS}
    pthread
    benchmark
)

target_include_directories(${PROJECT_NAME} PRIVATE
	../../synth
	../../synth/utils
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11 cxx_thread_local)

target_compile_options(${PROJECT_NAME}
    PRIVATE $<$<STREQUAL:$<CONFIGURATION>,Release>:
         -ftree-vectorize -ftree-vectorizer-verbose=1 -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -fno-unsafe-math-optimizations -Wno-psabi > #-Wno-psabi in gcc 7.1 https://stackoverflow.com/a/48149400
    PRIVATE $<$<STREQUAL:$<CONFIGURATION>,Debug>:
        -Wall >
)
