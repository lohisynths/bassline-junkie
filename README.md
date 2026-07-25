# Bassline Junkie

![Bassline Junkie synthesizer](docs/bassline-junkie.jpg)

Bassline Junkie is the real-time C++ synthesizer engine for the Lohi Bassline
Junkie instrument. It renders an eight-voice, three-oscillator synthesizer,
receives MIDI and control-change data from the hardware control surface over
UART, and sends stereo audio to an ALSA playback device.

The engine is designed for embedded Linux, including 64-bit ARM systems built
with Lohi Buildroot, but it can also be built on a Linux development machine.

## Features

- Eight polyphonic voices distributed across three worker threads
- Three wavetable oscillators per voice
- ADSR envelopes and LFO modulation
- State-variable filtering
- Oversampled soft clipping
- MIDI note and control-change parsing
- UART communication with the control-surface firmware
- Low-latency ALSA playback with CPU affinity and real-time scheduling
- DSP sweep and benchmark utilities

## Requirements

- Linux
- CMake 3.12 or newer
- A compiler with C++11 support (GCC or Clang)
- ALSA development files
- POSIX threads

On Debian or Ubuntu, install the host-build dependencies with:

```sh
sudo apt install build-essential cmake libasound2-dev
```

The optional test and benchmark build fetches Google Benchmark and GoogleTest
from GitHub during CMake configuration, so it also requires Git and an internet
connection.

## Build

The supplied build script accepts a compiler and configuration:

```sh
./build.sh gcc Release
./build.sh gcc Debug
./build.sh clang Release
./build.sh clang Debug
```

`Release` produces a CMake `RelWithDebInfo` build. The resulting executable is:

```text
build/bassline-junkie/src/synth/bassline-junkie
```

You can also use CMake directly:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build --parallel
```

## Cross-compile for ARM64

The repository includes a CMake toolchain file for a Lohi Buildroot-generated
ARM64 toolchain. Point `TOOLCHAIN_PATH_DIR` at Buildroot's `output/host`
directory and invoke the build script:

```sh
export TOOLCHAIN_PATH_DIR=/path/to/lohi-buildroot/output/host
./build.sh arm64 Release
```

The toolchain file expects the ARM64 compiler and sysroot layout produced by
the related Lohi Buildroot project.

## Run

Start the synthesizer with:

```sh
./build/bassline-junkie/src/synth/bassline-junkie
```

Press `Ctrl+C` to stop it cleanly.

At runtime, the engine expects:

- ALSA stereo playback at 44.1 kHz using signed 32-bit samples
- the `default` ALSA device on a development host
- `hw:0,0` on ARM or ARM64
- the control surface on `/dev/ttyAMA0`, configured for 1,000,000 baud
- at least four logical CPU cores for the configured thread affinities

The program attempts to lock memory and use `SCHED_FIFO` real-time scheduling.
Without the corresponding system permissions it reports an error and
continues, but audio performance may be reduced. If the serial device is not
available, the engine continues without hardware MIDI/control input.

Audio device names, sample rate, period size, voice count, and CPU allocation
are currently compile-time settings in
[`bassline-junkie/src/synth/config.h`](bassline-junkie/src/synth/config.h) and
[`bassline-junkie/src/synth/utils/AudioDevice.h`](bassline-junkie/src/synth/utils/AudioDevice.h).

## DSP utilities and benchmarks

Enable the additional targets during configuration:

```sh
cmake -S . -B build-tests \
  -DCMAKE_BUILD_TYPE=Release \
  -DBASSLINE_JUNKIE_TESTING_ENABLED=ON
cmake --build build-tests --parallel
```

This adds the following executables:

- `render_sweeps` - renders oscillator sweeps for analysis
- `soft_clipper_sweep` - renders a soft-clipper test sweep
- `sin_benchmark` - compares sine implementations
- `osc_benchmark` - benchmarks oscillator implementations

The scripts under `bassline-junkie/scripts/` and the test directories use
Python, NumPy, and Matplotlib to plot generated waveform and spectrum data.

## Repository layout

```text
.
├── bassline-junkie/
│   ├── scripts/              # Plotting and Pure Data utilities
│   └── src/
│       ├── synth/            # Synth engine, DSP, MIDI, serial, and audio code
│       └── tests/            # DSP renderers and benchmarks
├── cmake/                    # Dependencies and ARM64 toolchain configuration
├── docs/                     # Project media
├── build.sh                  # Host and cross-build helper
└── CMakeLists.txt
```

## Related projects

- [Bassline Junkie](https://github.com/lohisynths/bassline-junkie) - main
  Bassline Junkie project repository (this repository)
- [Bassline Junkie Interface v2](https://github.com/lohisynths/bassline-junkie-interface_v2) - control-surface firmware repository
- [Lohi Buildroot](https://github.com/lohisynths/lohi-buildroot) - Buildroot
  support for Lohi systems
