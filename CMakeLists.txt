
PROJECT(controldev)

cmake_minimum_required(VERSION 2.6)

#add definitions, compiler switches, etc.
ADD_DEFINITIONS(-Wall -O2)

find_package(PkgConfig)

ADD_LIBRARY(controldev SHARED JoyPad.cpp Joystick.cpp SliderBox.cpp LogitechG27.cpp)
TARGET_LINK_LIBRARIES(controldev sysfs usb pthread)

ADD_EXECUTABLE(sliderboxTest testSB.cpp)
TARGET_LINK_LIBRARIES(sliderboxTest controldev)

ADD_EXECUTABLE(joystickTest JoystickTest.cpp)
TARGET_LINK_LIBRARIES(joystickTest controldev)

ADD_EXECUTABLE(logitechG27Test LogitechG27Test.cpp)
TARGET_LINK_LIBRARIES(logitechG27Test controldev)

CONFIGURE_FILE(Doxyfile.in Doxyfile @ONLY)
ADD_CUSTOM_TARGET(doc doxygen Doxyfile)
INSTALL(TARGETS controldev
	RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib)
INSTALL(FILES LogitechG27.hpp Joystick.hpp JoyPad.hpp SliderBox.hpp DESTINATION include)

CONFIGURE_FILE(controldev.pc.in controldev.pc @ONLY)
INSTALL(FILES ${CMAKE_BINARY_DIR}/controldev.pc DESTINATION lib/pkgconfig)
