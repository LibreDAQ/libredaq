
# Include lib headers:
include_directories("${libldaq_SOURCE_DIR}/include")

# === Declare examples ===
MACRO(DECLARE_EXAMPLE _NAME)
add_executable(${_NAME} "${_NAME}.cpp")
target_link_libraries(
	${_NAME}
	ldaq  # LibreDAQ lib
	)
ENDMACRO()

# List examples
DECLARE_EXAMPLE(libredaq_example_adc)
DECLARE_EXAMPLE(libredaq_example_encoders)

	