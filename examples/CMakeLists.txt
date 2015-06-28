cmake_minimum_required(VERSION 2.8.3)
project(libredaq_examples)

# Include lib headers:
#include_directories("${libldaq_SOURCE_DIR}/include")
FIND_PACKAGE(libldaq REQUIRED)

# === Declare examples ===
MACRO(DECLARE_EXAMPLE _NAME)
	add_executable(${_NAME} "${_NAME}.cpp")
	target_link_libraries(
		${_NAME}
		${libldaq_LIBRARIES}
		)
	if(TARGET ldaq)
		add_dependencies(${_NAME} ldaq)
	endif()
ENDMACRO()

# List examples
DECLARE_EXAMPLE(libredaq_example_adc)
DECLARE_EXAMPLE(libredaq_example_encoders)
DECLARE_EXAMPLE(libredaq_example_dac)
DECLARE_EXAMPLE(libredaq_example_pga_adc)
	