cmake_minimum_required(VERSION 3.5)

project(bassline-junkie VERSION 0.0.0 LANGUAGES CXX)
find_package(ALSA REQUIRED)
if ( NOT ALSA )
    message(MESSAGE_FATAL "The process is not interrupted!")
endif ( NOT ALSA )

add_subdirectory(src/tests)

set(CMAKE_VERBOSE_MAKEFILE ON)
