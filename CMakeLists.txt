
PROJECT(controldev)

cmake_minimum_required(VERSION 2.6)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

#add definitions, compiler switches, etc.
ADD_DEFINITIONS(-Wall -O2)

find_package(PkgConfig)

#pkg_check_modules(DFKI_TYPES REQUIRED "dfki-types")
#INCLUDE_DIRECTORIES(${DFKI_TYPES_INCLUDE_DIRS})
#LINK_DIRECTORIES(${DFKI_TYPES_LIBRARY_DIRS})

ADD_LIBRARY(controldev SHARED Joystick.cpp SliderBox.cpp)
TARGET_LINK_LIBRARIES(controldev ${DFKI_TYPES_LIBS})

CONFIGURE_FILE(Doxyfile.in Doxyfile @ONLY)
ADD_CUSTOM_TARGET(doc doxygen Doxyfile)
INSTALL(TARGETS controldev
	RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib)
INSTALL(FILES Joystick.h SliderBox.h DESTINATION include)

CONFIGURE_FILE(controldev.pc.in controldev.pc @ONLY)
INSTALL(FILES ${CMAKE_BINARY_DIR}/controldev.pc DESTINATION lib/pkgconfig)
