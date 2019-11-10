cmake_minimum_required(VERSION 3.5)

project(bassline-junkie VERSION 0.0.0 LANGUAGES CXX)
find_package(ALSA REQUIRED)
if( NOT ALSA_FOUND )
    #message(FATAL_ERROR "The process is not interrupted!")
endif()

# Create the executable
add_executable(${PROJECT_NAME}
	bassline-junkie.cpp
	dsp/VAStateVariableFilter.cpp
	dsp/Lfo.cpp
	dsp/Osc.cpp
	dsp/PolyBLEPOscillator/PolyBLEPOscillator.cpp
	dsp/PolyBLEPOscillator/Oscillator.cpp
	dsp/MoogFilter.cpp
	dsp/ADSR/ADSR.cpp
	utils/array_writer.cpp
	utils/SerialReceiver.cpp
	utils/cpucounter.cpp
	utils/MidiReceiver.cpp
	utils/MidiParser.cpp
)

if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES armv7l)
    set(ADITIONAL_LIBS jack)
endif()

target_link_libraries(${PROJECT_NAME}
    asound
    pthread
    ${ADITIONAL_LIBS}
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11 cxx_thread_local)

target_compile_options(${PROJECT_NAME}
    PRIVATE $<$<STREQUAL:$<CONFIGURATION>,Release>:
        -ffast-math -fno-unsafe-math-optimizations -Wall -Wno-psabi > #-Wno-psabi in gcc 7.1 https://stackoverflow.com/a/48149400
    PRIVATE $<$<STREQUAL:$<CONFIGURATION>,Debug>:
        -Wall >
)
