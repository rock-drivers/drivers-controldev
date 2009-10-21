
PROJECT(controldev)

cmake_minimum_required(VERSION 2.6)

#add definitions, compiler switches, etc.
ADD_DEFINITIONS(-Wall -O2)

find_package(PkgConfig)

ADD_LIBRARY(controldev SHARED Joystick.cpp SliderBox.cpp)
TARGET_LINK_LIBRARIES(controldev sysfs)

CONFIGURE_FILE(Doxyfile.in Doxyfile @ONLY)
ADD_CUSTOM_TARGET(doc doxygen Doxyfile)
INSTALL(TARGETS controldev
	RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib)
INSTALL(FILES Joystick.hpp SliderBox.hpp DESTINATION include)

CONFIGURE_FILE(controldev.pc.in controldev.pc @ONLY)
INSTALL(FILES ${CMAKE_BINARY_DIR}/controldev.pc DESTINATION lib/pkgconfig)
