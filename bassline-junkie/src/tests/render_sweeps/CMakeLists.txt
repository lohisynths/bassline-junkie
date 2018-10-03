cmake_minimum_required(VERSION 3.5)

project(render_sweeps VERSION 0.0.0 LANGUAGES CXX)
find_package(ALSA REQUIRED)
if ( NOT ALSA )
    message(MESSAGE_FATAL "The process is not interrupted!")
endif ( NOT ALSA )

# Create the executable
add_executable(${PROJECT_NAME}
	../../synth/dsp/PolyBLEPOscillator/Oscillator.cpp
	../../synth/dsp/PolyBLEPOscillator/PolyBLEPOscillator.cpp
	sweep.cpp
)

# Link the static library from subproject1 using it's alias sub::lib1
# Link the header only library from subproject2 using it's alias sub::lib2
# This will cause the include directories for that target to be added to this project
target_link_libraries(${PROJECT_NAME}
    lohi::stk
    asound
    jack
    pthread
)

target_include_directories(${PROJECT_NAME} PRIVATE
	../../synth
	../../synth/utils
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11 cxx_thread_local)

set(CMAKE_VERBOSE_MAKEFILE ON)
