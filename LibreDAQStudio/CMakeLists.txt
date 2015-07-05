cmake_minimum_required(VERSION 2.8.11)
project(LibreDAQStudio)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets QUIET)  # Find the QtWidgets library
IF (NOT Qt5Widgets_FOUND)
	MESSAGE(STATUS "*WARNING* Qt5 not found. Cannot build LibreDAQStudio. \n  Add the installation prefix of Qt5Widgets to CMAKE_PREFIX_PATH \n  or set Qt5Widgets_DIR to a directory containing one of the above files, typically: 'C:\Qt\5.X\msvcXXXX_64\lib\cmake\Qt5Widgets'")
ELSE()
	FIND_PACKAGE(libldaq REQUIRED)

	set(FORMS
		mainwindow.ui
	)
	qt5_wrap_ui(FORMS_HEADERS ${FORMS})

	set(RESOURCES
		libredaqstudio.qrc
	)
	qt5_add_resources(RESOURCES_RCC ${RESOURCES})

	# Tell CMake to create the helloworld executable
	add_executable(LibreDAQStudio WIN32 
		main.cpp
		mainwindow.cpp mainwindow.h
		${FORMS_HEADERS}
		${RESOURCES_RCC}
		)

	# Use the Widgets module from Qt 5.
	target_link_libraries(LibreDAQStudio
		Qt5::Widgets 
		${libldaq_LIBRARIES})

	# for builds inside the main LibreDAQ SDK
	if(TARGET ldaq)
		add_dependencies(LibreDAQStudio ldaq)
	endif()
ENDIF() # if qt not found
