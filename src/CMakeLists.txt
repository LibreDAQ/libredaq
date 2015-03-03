
file(GLOB LST_SRCS "${libredaq_SOURCE_DIR}/src/*.cpp")
file(GLOB LST_INT_HDRS "${libredaq_SOURCE_DIR}/src/*.h")
file(GLOB LST_PUB_HDRS "${libredaq_SOURCE_DIR}/include/*.h")

file(GLOB_RECURSE LST_UTILS_SRCS "${libredaq_SOURCE_DIR}/src/utils/*.cpp")
file(GLOB_RECURSE LST_UTILS_HDRS "${libredaq_SOURCE_DIR}/src/utils/*.h")

include_directories("${libredaq_SOURCE_DIR}/src")

SOURCE_GROUP("Public API .h" FILES ${LST_PUB_HDRS})
SOURCE_GROUP("libreDAQ lib" FILES ${LST_SRCS} ${LST_INT_HDRS})
SOURCE_GROUP("Utilities" FILES ${LST_UTILS_SRCS} ${LST_UTILS_HDRS})

add_library(
	ldaq
	${LST_PUB_HDRS}
	${LST_SRCS}	${LST_INT_HDRS}
	${LST_UTILS_SRCS} ${LST_UTILS_HDRS}
	)
