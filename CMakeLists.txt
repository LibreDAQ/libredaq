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

# Declare library:
add_subdirectory(src)

# Examples:
