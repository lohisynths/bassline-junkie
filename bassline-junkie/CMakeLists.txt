cmake_minimum_required(VERSION 3.5)

project(bassline-junkie VERSION 0.0.0 LANGUAGES CXX)
find_package(ALSA REQUIRED)
if ( NOT ALSA )
    message(MESSAGE_FATAL "The process is not interrupted!")
endif ( NOT ALSA )



#message(MESSAGE_FATAL  ${BUILDROOT_ALSA_DIR})

# Create the executable
add_executable(${PROJECT_NAME}

src/synth/bassline-junkie.cpp
src/synth/dsp/VAStateVariableFilter.cpp
src/synth/dsp/Lfo.cpp
src/synth/dsp/Osc.cpp
src/synth/dsp/PolyBLEPOscillator/PolyBLEPOscillator.cpp
src/synth/dsp/PolyBLEPOscillator/Oscillator.cpp
src/synth/dsp/MoogFilter.cpp
src/synth/dsp/ADSR/ADSR.cpp
src/synth/utils/wavwriter.cpp
src/synth/utils/MidiReceiverRt.cpp
src/synth/utils/array_writer.cpp
src/synth/utils/SerialReceiver.cpp
src/synth/utils/cpucounter.cpp
src/synth/utils/MidiReceiver.cpp
src/synth/utils/MidiParser.cpp
src/synth/utils/AudioDevice.cpp

)

# Link the static library from subproject1 using it's alias sub::lib1
# Link the header only library from subproject2 using it's alias sub::lib2
# This will cause the include directories for that target to be added to this project
target_link_libraries(${PROJECT_NAME}
    lohi::stk
    asound
    pthread
    jack
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11 cxx_thread_local)

add_subdirectory(src/tests)

set(CMAKE_VERBOSE_MAKEFILE ON)
