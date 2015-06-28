# --------------------------------------------------------------
#   LibreDAQ PC interface library
#   See: xxxx
#   (C) 2015 
#   License GNU GPL 3
# --------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.3)
project(libredaq)

SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS 1)

# Version numbers:
# ======================================
set(CMAKE_LIBREDAQ_VERSION_NUMBER_MAJOR 0)
set(CMAKE_LIBREDAQ_VERSION_NUMBER_MINOR 0)
set(CMAKE_LIBREDAQ_VERSION_NUMBER_PATCH 1)
IF(WIN32)
	SET(LIBLDAQ_DLL_VERSION_POSTFIX "${CMAKE_LIBREDAQ_VERSION_NUMBER_MAJOR}${CMAKE_LIBREDAQ_VERSION_NUMBER_MINOR}${CMAKE_LIBREDAQ_VERSION_NUMBER_PATCH}")
ELSE()
	SET(LIBLDAQ_DLL_VERSION_POSTFIX "")
ENDIF()


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

# ======================================
#  C++ Library
# ======================================
add_subdirectory(libldaq)

# ======================================
# App: LibreDAQStudio
# ======================================
add_subdirectory(LibreDAQStudio)

# ======================================
# Examples:
# ======================================
add_subdirectory(examples)

