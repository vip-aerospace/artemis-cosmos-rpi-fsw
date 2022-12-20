# Notes:
# - make sure you are using a g++ cross compiler for arm, version 4.8 or above for c++11 compatibility
# - when cross compiling use the variable CMAKE_TOOLCHAIN_FILE
#       Ex.: -DCMAKE_TOOLCHAIN_FILE="E:/cosmos-source/projects/neutron1/source/cmake/toolchain_arm_win"
#       also note that CMAKE_SYSTEM_NAME is the target platform (ex: Linux) and CMAKE_HOST_SYSTEM_NAME is the
#       platform where the code is being compiled (Ex: Windows)

#--------------------------------------------------------------------
# user config variables
#--------------------------------------------------------------------
cmake_minimum_required(VERSION 3.6)

set (CMAKE_CXX_STANDARD 17)

if(CROSS_TYPE)
    message("Compiling for ${CROSS_TYPE}")
else()
    message("Compiling for x86")
    SET(CROSS_TYPE "x86")
endif()
#SET(COSMOS_SOURCE_CORE "C:/cosmos/src/core/")
#include(${COSMOS_SOURCE}/core/cmake/toolchain-win-to-arm-sysgcc.cmake)


# define the project names
SET(PROJECT_NAME artemis-flight)

IF (${CMAKE_BUILD_TYPE} MATCHES "Debug")
MESSAGE("Building Debug version")
IF (${CROSS_TYPE} MATCHES "arm" OR ${CROSS_TYPE} MATCHES "iobc" OR ${CROSS_TYPE} MATCHES "rpi")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-2")
else()
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
endif()
ENDIF()

IF (${CMAKE_BUILD_TYPE} MATCHES "Release")
MESSAGE("Building Release version")
ENDIF()

# options: all, none, ...
# SET(ADD_EXECUTABLES "all")

# define folder where your programs are going to be installed
#SET(INSTALL_DIR                run)

#--------------------------------------------------------------------
# automatic config section, you should not have to edit further
#--------------------------------------------------------------------
# comment
#SET(CMAKE_LEGACY_CYGWIN_WIN32 0) # Remove when CMake >= 2.8.4 is required
cmake_minimum_required(VERSION 3.1)
project(${PROJECT_NAME})

Option(BUILD_GPROF "Build for Profiling" OFF)

IF (BUILD_GPROF)
MESSAGE("Building for gprof Profiler")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -Wall -Wmaybe-uninitialized -Wtype-limits -pedantic -std=c++17 -U__STRICT_ANSI__ -Wno-invalid-offsetof")
ELSE ()
set(COSMOS_CXX_FLAGS "-Os -Wmaybe-uninitialized -Wtype-limits -Wall -pedantic -std=c++17 -U__STRICT_ANSI__ -Wno-invalid-offsetof -Wno-format -Wno-date-time")
ENDIF ()


include(cmake/macros.cmake)

# -----------------------------------------------
set(COSMOS_SOURCE ${PROJECT_SOURCE_DIR}/../../..)
message ("COSMOS_SOURCE: ${COSMOS_SOURCE}")

# compile on Linux host
IF(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")

MESSAGE("Compiling on Linux for ${CROSS_TYPE}")
#SET( "${COSMOS_SOURCE}/core/")

# Linux specific code
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

#Check for ARM Build
IF (${CROSS_TYPE} MATCHES "arm" OR ${CROSS_TYPE} MATCHES "iobc" OR ${CROSS_TYPE} MATCHES "rpi")

# ARM Build Detected

# Set installation directory
IF("$ENV{COSMOS}" STREQUAL "")
SET(COSMOS "~/cosmos/${CROSS_TYPE}")
message("ARM Default COSMOS: ${COSMOS}")
ELSE()
SET(COSMOS "$ENV{COSMOS}/${CROSS_TYPE}")
message("ARM Environment COSMOS: ${COSMOS}")
ENDIF("$ENV{COSMOS}" STREQUAL "")

# Set Toolchain File
SET(CMAKE_TOOLCHAIN_FILE "${PROJECT_SOURCE_DIR}/build/toolchain_${CROSS_TYPE}")

# Add preprocessor definition to help source files determine code to use for ARMv7 specific implementation
add_definitions(-DCROSS_TYPE_arm) #20150825JC: Added definition

# Add include directories for locally compiled arm openssl
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/arm/include)

ELSE()

# Non-ARM Build Detected

IF("$ENV{COSMOS}" STREQUAL "")
SET(COSMOS "~/cosmos")
ELSE()
SET(COSMOS "$ENV{COSMOS}")
ENDIF("$ENV{COSMOS}" STREQUAL "")

ENDIF()

ENDIF(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")



# -----------------------------------------------

set(USE_COSMOS_FROM "SOURCE")
include(${COSMOS_SOURCE}/core/cmake/use_cosmos_from_source.cmake)


# let's use the cosmos/core libraries

#set(COSMOS_LIBS CosmosDeviceI2C CosmosDeviceDisk CosmosDeviceGeneral CosmosAgent CosmosDeviceSerial CosmosPhysics CosmosSupport CosmosMath localzlib ${WINDOWS_LIBS})
set(COSMOS_LIBS ${WINDOWS_LIBS})

set(ARTEMIS_LIBS ARTEMIS_HDLC)

message("COSMOS: ${COSMOS}")

set(CMAKE_INSTALL_PREFIX "${COSMOS}/${PROJECT_NAME}" CACHE PATH "default install path" FORCE )
message("Install: ${CMAKE_INSTALL_PREFIX}")

include_directories(${COSMOS_SOURCE}/core/libraries)

# -----------------------------------------------

# Add external modules


# Note that the modules folder must be cloned/added in manually

# include_directories(${COSMOS_SOURCE}/modules/devices/isis/libraries)
# add_subdirectory(${COSMOS_SOURCE}/modules/devices/isis/libraries     ${CMAKE_BINARY_DIR}/libraries/isis)

# include_directories(${COSMOS_SOURCE}/modules/devices/radio/astrodev/libraries)
# add_subdirectory(${COSMOS_SOURCE}/modules/devices/radio/astrodev/libraries     ${CMAKE_BINARY_DIR}/libraries/astrodev)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBCAMERA REQUIRED libcamera)
message(STATUS "libcamera library found:")
message(STATUS "    version: ${LIBCAMERA_VERSION}")
message(STATUS "    libraries: ${LIBCAMERA_LINK_LIBRARIES}")
message(STATUS "    include path: ${LIBCAMERA_INCLUDE_DIRS}")
include_directories(${CMAKE_SOURCE_DIR} ${LIBCAMERA_INCLUDE_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/libraries)
add_subdirectory(${PROJECT_SOURCE_DIR}/libraries)

# add the Programs
add_subdirectory(${PROJECT_SOURCE_DIR}/programs)

# 20150826JC: Added to confirm flags
MESSAGE("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
MESSAGE("CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
