cmake_minimum_required(VERSION 3.5)

project(bassline-junkie VERSION 0.0.0 LANGUAGES CXX)
find_package(ALSA REQUIRED)
if( NOT ALSA_FOUND )
    #message(FATAL_ERROR "The process is not interrupted!")
endif()

# Create the executable
add_executable(${PROJECT_NAME}
	bassline-junkie.cpp
	dsp/BlitSaw.cpp
	dsp/BlitSquare.cpp
	dsp/VAStateVariableFilter.cpp
	dsp/Lfo.cpp
	dsp/Noise.cpp
	dsp/Osc.cpp
	dsp/PolyBLEPOscillator/PolyBLEPOscillator.cpp
	dsp/PolyBLEPOscillator/Oscillator.cpp
	dsp/SineWave.cpp
	dsp/WavetableOscillator.cpp
	dsp/MoogFilter.cpp
	dsp/ADSR/ADSR.cpp
	utils/array_writer.cpp
	utils/SerialReceiver.cpp
	utils/cpucounter.cpp
	utils/MidiReceiver.cpp
	utils/MidiParser.cpp
)

target_link_libraries(${PROJECT_NAME}
    asound
    pthread
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11 cxx_thread_local)

target_compile_options(${PROJECT_NAME}
    PRIVATE $<$<STREQUAL:$<CONFIGURATION>,RelWithDebInfo>:
        -Ofast -flto -Wall -Wextra -Wpedantic>
    PRIVATE $<$<STREQUAL:$<CONFIGURATION>,Debug>:
        -Wall -Wextra -Wpedantic>
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_FLAGS_RELWITHDEBINFO "-flto"
    LINK_FLAGS_RELEASE "-flto"
)
