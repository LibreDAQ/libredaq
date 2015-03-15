# --------------------------------------------------------------
#   LibreDAQ PC interface library
#   See: xxxx
#   (C) 2015 
#   License GNU GPL 3
# --------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.3)
project(libredaq)

set(CMAKE_LIBREDAQ_VERSION_NUMBER_MAJOR 0)
set(CMAKE_LIBREDAQ_VERSION_NUMBER_MINOR 0)
set(CMAKE_LIBREDAQ_VERSION_NUMBER_PATCH 1)

include_directories("${libredaq_SOURCE_DIR}/include")

# Shared options between GCC and CLANG:
# ======================================
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR CMAKE_COMPILER_IS_GNUCXX)
    # We need pthread's
    IF (UNIX AND NOT APPLE)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    ENDIF()

    IF(MRPT_OPTIMIZE_NATIVE AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
      SET(EXTRA_CPP_FLAGS "${EXTRA_CPP_FLAGS} -O3")
    ENDIF()
endif()



# Declare library:
add_subdirectory(src)

# Examples:
add_subdirectory(examples)

