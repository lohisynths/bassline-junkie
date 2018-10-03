cmake_minimum_required(VERSION 3.5)

project(render_sweeps VERSION 0.0.0 LANGUAGES CXX)
find_package(ALSA REQUIRED)
if(NOT ALSA_FOUND )
    message(MESSAGE_FATAL "The process is not interrupted!")
endif()

# Create the executable
add_executable(${PROJECT_NAME}
	../../synth/dsp/PolyBLEPOscillator/Oscillator.cpp
	../../synth/dsp/PolyBLEPOscillator/PolyBLEPOscillator.cpp
	bench.cpp
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
