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

	