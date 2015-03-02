# --------------------------------------------------------------
#   LibreDAQ PC interface library
#   See: xxxx
#   (C) 2015 
#   License GNU GPL 3
# --------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.3)
project(libredaq)

include_directories("include")

# Declare library:
file(GLOB LST_SRCS "src/*.cpp")
file(GLOB LST_INT_HDRS "src/*.h")
file(GLOB LST_PUB_HDRS "include/*.h")

add_library(
	ldaq
	${LST_SRCS}
	${LST_INT_HDRS}
	${LST_PUB_HDRS}
	)

# Examples:

