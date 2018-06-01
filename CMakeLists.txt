# --------------------------------------------------------------
#   LibreDAQ PC interface library
#   (C) 2015-2018
#   License GNU GPL 3
# --------------------------------------------------------------
CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
set (CMAKE_CXX_STANDARD 14)  # Require C++14
project(libredaq)

option(BUILD_ROS "Build ROS node" ON)

# Version numbers:
# ======================================
set(CMAKE_LIBREDAQ_VERSION_NUMBER_MAJOR 0)
set(CMAKE_LIBREDAQ_VERSION_NUMBER_MINOR 0)
set(CMAKE_LIBREDAQ_VERSION_NUMBER_PATCH 1)
if(WIN32)
	set(LIBLDAQ_DLL_VERSION_POSTFIX "${CMAKE_LIBREDAQ_VERSION_NUMBER_MAJOR}${CMAKE_LIBREDAQ_VERSION_NUMBER_MINOR}${CMAKE_LIBREDAQ_VERSION_NUMBER_PATCH}")
else()
	set(LIBLDAQ_DLL_VERSION_POSTFIX "")
endif()


# Shared options between GCC and CLANG:
# ======================================
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR CMAKE_COMPILER_IS_GNUCXX)
    # We need pthread's
    IF (UNIX AND NOT APPLE)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    endif()

	add_compile_options(-Wall)

    if(MRPT_OPTIMIZE_NATIVE AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
      set(EXTRA_CPP_FLAGS "${EXTRA_CPP_FLAGS} -O3")
    endif()
endif()

# ======================================
#  C++ Library
# ======================================
add_subdirectory(libldaq)
set(libldaq_DIR "${libredaq_BINARY_DIR}")

# ======================================
# App: LibreDAQStudio
# ======================================
add_subdirectory(LibreDAQStudio)

# ======================================
# Examples:
# ======================================
add_subdirectory(examples)


# ======================================
#  ROS
# ======================================
if (BUILD_ROS)
	## Find catkin macros and libraries
	## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
	## is used, also find other catkin packages
	find_package(catkin REQUIRED COMPONENTS
	  roscpp
	  std_msgs
	  message_generation
	  sensor_msgs
	)

	################################################
	## Declare ROS messages, services and actions ##
	################################################
	add_message_files(
	  DIRECTORY msg
	  FILES EncoderDecimatedSpeed.msg
	)
	generate_messages(DEPENDENCIES std_msgs)

	###################################
	## catkin specific configuration ##
	###################################
	## The catkin_package macro generates cmake config files for your package
	## Declare things to be passed to dependent projects
	## INCLUDE_DIRS: uncomment this if you package contains header files
	## LIBRARIES: libraries you create in this project that dependent projects also need
	## CATKIN_DEPENDS: catkin_packages dependent projects also need
	## DEPENDS: system dependencies of this project that dependent projects also need
	catkin_package(
		# INCLUDE_DIRS include
		# LIBRARIES ${PROJECT_NAME}
		CATKIN_DEPENDS std_msgs message_runtime sensor_msgs
		# DEPENDS system_lib
	)

	###########
	## Build ##
	###########
	## Specify additional locations of header files
	## Your package locations should be listed before other locations
	# include_directories(include)
	include_directories(
	  ${catkin_INCLUDE_DIRS}
	)

	# Declare a C++ executable
	add_executable(${PROJECT_NAME}_node ros-node/${PROJECT_NAME}_node.cpp)

	FIND_PACKAGE(libldaq REQUIRED)

	# Add cmake target dependencies of the executable
	# same as for the library above
	add_dependencies(${PROJECT_NAME}_node
		ldaq
		${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS}
		${PROJECT_NAME}_generate_messages
		)

	# Specify libraries to link a library or executable target against
	target_link_libraries(${PROJECT_NAME}_node
	   ${catkin_LIBRARIES}
	   ${libldaq_LIBRARIES}
	)

	#############
	## Install ##
	#############

	# all install targets should use catkin DESTINATION variables
	# See http://ros.org/doc/api/catkin/html/adv_user_guide/variables.html

	## Mark executable scripts (Python etc.) for installation
	## in contrast to setup.py, you can choose the destination
	# install(PROGRAMS
	#   scripts/my_python_script
	#   DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
	# )

	# Mark executables and/or libraries for installation
	install(TARGETS ${PROJECT_NAME}_node
	   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
	)
endif() #end ROS
