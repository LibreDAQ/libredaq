
file(GLOB LST_SRCS "${libldaq_SOURCE_DIR}/src/*.cpp")
file(GLOB LST_INT_HDRS "${libldaq_SOURCE_DIR}/src/*.h")
file(GLOB LST_PUB_HDRS "${libldaq_SOURCE_DIR}/include/*.h")

file(GLOB_RECURSE LST_UTILS_SRCS "${libldaq_SOURCE_DIR}/src/utils/*.cpp")
file(GLOB_RECURSE LST_UTILS_HDRS "${libldaq_SOURCE_DIR}/src/utils/*.h")

include_directories("${libldaq_SOURCE_DIR}/include")
include_directories("${libldaq_SOURCE_DIR}/src")

SOURCE_GROUP("Public API .h" FILES ${LST_PUB_HDRS})
SOURCE_GROUP("libreDAQ lib" FILES ${LST_SRCS} ${LST_INT_HDRS})
SOURCE_GROUP("Utilities" FILES ${LST_UTILS_SRCS} ${LST_UTILS_HDRS})

# ------------------------------------------
# We want libraries to be named "libXXX" 
#  and "libXXX-dbg" in all compilers:
# ------------------------------------------
set(CMAKE_DEBUG_POSTFIX  "-dbg")
IF(MSVC)
	set(MY_LIB_PREFIX "lib")  # Libs are: "libXXX"
ENDIF()

add_library(
	ldaq
	${LST_PUB_HDRS}
	${LST_SRCS}	${LST_INT_HDRS}
	${LST_UTILS_SRCS} ${LST_UTILS_HDRS}
	)

SET_TARGET_PROPERTIES(ldaq PROPERTIES 
	OUTPUT_NAME ${MY_LIB_PREFIX}ldaq${LIBLDAQ_DLL_VERSION_POSTFIX}
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/"
	VERSION "${CMAKE_LIBREDAQ_VERSION_NUMBER_MAJOR}.${CMAKE_LIBREDAQ_VERSION_NUMBER_MINOR}.${CMAKE_LIBREDAQ_VERSION_NUMBER_PATCH}"
	SOVERSION ${CMAKE_LIBREDAQ_VERSION_NUMBER_MAJOR}.${CMAKE_LIBREDAQ_VERSION_NUMBER_MINOR}
	)

# ---------------------------------------------
#   CMake config file: 1/2 for local usage
# ---------------------------------------------
SET(THE_libldaq_SOURCE_DIR "${libldaq_SOURCE_DIR}")
SET(THE_libldaq_LIBS_INCL_DIR "${libldaq_SOURCE_DIR}/include")
SET(THE_CMAKE_BINARY_DIR "${CMAKE_BINARY_DIR}")
SET(ldaq_CONFIGFILE_IS_INSTALL 0)

CONFIGURE_FILE(
	"${libldaq_SOURCE_DIR}/libldaq-config.cmake.in"
    "${libredaq_BINARY_DIR}/libldaq-config.cmake" @ONLY IMMEDIATE )
#support for version checking when finding MRPT, e.g. find_package(MRPT 1.0.0 EXACT)
CONFIGURE_FILE(
	"${libldaq_SOURCE_DIR}/libldaq-config-version.cmake.in"
    "${libredaq_BINARY_DIR}/libldaq-config-version.cmake" @ONLY IMMEDIATE )

# ---------------------------------------------
#   CMake config file: 1/2 for local usage
# ---------------------------------------------
SET(THE_libldaq_SOURCE_DIR "${libldaq_SOURCE_DIR}")
SET(THE_libldaq_LIBS_INCL_DIR "${libldaq_SOURCE_DIR}/include")
SET(THE_CMAKE_BINARY_DIR "${CMAKE_BINARY_DIR}")
SET(ldaq_CONFIGFILE_IS_INSTALL 0)

CONFIGURE_FILE(
	"${libldaq_SOURCE_DIR}/libldaq-config.cmake.in"
    "${libredaq_BINARY_DIR}/libldaq-config.cmake" @ONLY IMMEDIATE )
#support for version checking when finding MRPT, e.g. find_package(MRPT 1.0.0 EXACT)
CONFIGURE_FILE(
	"${libldaq_SOURCE_DIR}/libldaq-config-version.cmake.in"
    "${libredaq_BINARY_DIR}/libldaq-config-version.cmake" @ONLY IMMEDIATE )

# --------------------------------------------------------------------
#   CMake config file: 2/2 for make install / Linux packages usage
# ---------------------------------------------------------------------
IF(WIN32)
	SET(THE_libldaq_SOURCE_DIR "\${THIS_libldaq_CONFIG_PATH}")
	SET(THE_libldaq_LIBS_INCL_DIR "${libldaq_SOURCE_DIR}/include")
	SET(THE_CMAKE_BINARY_DIR "\${THIS_libldaq_CONFIG_PATH}")
ELSE()
	# Unix install. This .cmake file will end up in /usr/share/mrpt/MRPTConfig.cmake :
	IF (CMAKE_IS_BUILDING_DEBIAN_PKG)
		# We're building a .deb package: DESTDIR is NOT the final installation directory:
		SET(THE_libldaq_SOURCE_DIR "/usr")
		SET(THE_libldaq_LIBS_INCL_DIR "${THE_MRPT_SOURCE_DIR}/include/")
		SET(THE_CMAKE_BINARY_DIR "/usr")
	ELSE()
		# Normal case: take the desired installation directory
		SET(THE_libldaq_SOURCE_DIR "${CMAKE_INSTALL_PREFIX}")
		SET(THE_libldaq_LIBS_INCL_DIR "${THE_MRPT_SOURCE_DIR}/include/")
		SET(THE_CMAKE_BINARY_DIR "${CMAKE_INSTALL_PREFIX}")
	ENDIF()
ENDIF()
SET(ldaq_CONFIGFILE_IS_INSTALL 1)

CONFIGURE_FILE(
	"${libldaq_SOURCE_DIR}/libldaq-config.cmake.in"
    "${libredaq_BINARY_DIR}/unix-install/libldaq-config.cmake" @ONLY IMMEDIATE )
#support for version checking when finding MRPT, e.g. find_package(MRPT 1.0.0 EXACT)
CONFIGURE_FILE(
	"${libldaq_SOURCE_DIR}/libldaq-config-version.cmake.in"
    "${libredaq_BINARY_DIR}/unix-install/libldaq-config-version.cmake" @ONLY IMMEDIATE )
