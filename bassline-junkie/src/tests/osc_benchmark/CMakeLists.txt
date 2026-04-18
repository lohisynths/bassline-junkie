cmake_minimum_required(VERSION 3.5)

project(osc_benchmark VERSION 0.0.0 LANGUAGES CXX)
find_package(ALSA REQUIRED)
if(NOT ALSA_FOUND )
    message(MESSAGE_FATAL "The process is not interrupted!")
endif()

# Create the executable
add_executable(${PROJECT_NAME}
	../../synth/dsp/BlitSaw.cpp
	../../synth/dsp/BlitSquare.cpp
	../../synth/dsp/MinBLEPOscillator.cpp
	../../synth/dsp/Noise.cpp
	../../synth/dsp/PolyBLEPOscillator/Oscillator.cpp
	../../synth/dsp/PolyBLEPOscillator/PolyBLEPOscillator.cpp
	../../synth/dsp/PolyBLEP/PolyBLEP.cpp
	../../synth/dsp/SineWave.cpp
	../../synth/dsp/WavetableOscillator.cpp
	osc_bench.cpp
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
