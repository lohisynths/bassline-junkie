# - Find libusb for portable USB support
# 
# If the LibUSB_ROOT environment variable
# is defined, it will be used as base path.
# The following standard variables get defined:
#  LibUSB_FOUND:    true if LibUSB was found
#  LibUSB_INCLUDE_DIR: the directory that contains the include file
#  LibUSB_LIBRARIES:  the libraries

if(NOT DEFINED ENV{TOOLCHAIN_PATH_DIR})
    message("TOOLCHAIN_PATH_DIR not defined")
    message(FATAL_ERROR "    example:  'export TOOLCHAIN_PATH_DIR=/home/alax/git/lohi_buildroot/output/host'")
endif()

set(CMAKE_TOOLCHAIN_PATH $ENV{TOOLCHAIN_PATH_DIR})



FIND_PACKAGE(PkgConfig REQUIRED)

set(BUILDROOT_SYSROOT "${RELOCATED_HOST_DIR}/arm-buildroot-linux-gnueabihf/sysroot")
set(BUILDROOT_PKG_CONFIG_LIBDIR "${BUILDROOT_SYSROOT}/usr/lib/pkgconfig:${CMAKE_TOOLCHAIN_PATH}/lib/pkgconfig")

set(ENV{PKG_CONFIG_DIR} "")
set(ENV{PKG_CONFIG_LIBDIR} ${BUILDROOT_PKG_CONFIG_LIBDIR})
set(ENV{PKG_CONFIG_SYSROOT_DIR} ${BUILDROOT_SYSROOT})



message(${BUILDROOT_PKG_CONFIG_LIBDIR})


IF(CMAKE_TOOLCHAIN_PATH)
  IF(DEPENDS_DIR) #Otherwise use System pkg-config path
    SET(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${DEPENDS_DIR}/libusb/lib/pkgconfig")
  ENDIF()
  SET(MODULE "alsa")
  IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
    SET(MODULE "alsa-1.0.0>=1.3.0")
  ENDIF()
  IF(LibUSB_FIND_REQUIRED)
    SET(LibUSB_REQUIRED "REQUIRED")
  ENDIF()
  PKG_CHECK_MODULES(LibUSB ${LibUSB_REQUIRED} ${MODULE})

  FIND_LIBRARY(LibUSB_LIBRARY
    NAMES ${LibUSB_LIBRARIES}
    HINTS ${LibUSB_LIBRARY_DIRS}
  )
  SET(LibUSB_LIBRARIES ${LibUSB_LIBRARY})

  RETURN()
ENDIF()

