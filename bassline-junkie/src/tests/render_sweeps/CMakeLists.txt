cmake_minimum_required(VERSION 3.12)

project(render_sweeps VERSION 0.0.0 LANGUAGES CXX)
find_package(ALSA REQUIRED)
find_package(Threads REQUIRED)

# Create the executable
add_executable(${PROJECT_NAME}
	../../synth/dsp/BlitSaw.cpp
	../../synth/dsp/BlitSquare.cpp
	../../synth/dsp/MinBLEPOscillator.cpp
	../../synth/dsp/Noise.cpp
	../../synth/dsp/Osc.cpp
	../../synth/dsp/PolyBLEPOscillator/Oscillator.cpp
	../../synth/dsp/PolyBLEPOscillator/PolyBLEPOscillator.cpp
	../../synth/dsp/PolyBLEP/PolyBLEP.cpp
	../../synth/dsp/SineWave.cpp
	../../synth/dsp/WavetableOscillator.cpp
	sweep.cpp
)


target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ALSA::ALSA
        Threads::Threads
        benchmark
)

target_include_directories(${PROJECT_NAME} PRIVATE
	../../synth
	../../synth/utils
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11 cxx_thread_local)
