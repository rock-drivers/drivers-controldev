rock_library(controldev
    SOURCES JoyPad.cpp  
            Joystick.cpp
	    LogitechG27.cpp  
	    SliderBox.cpp 
            ConnexionHID.cpp
            GraupnerMC20.cpp
    HEADERS JoyPad.hpp  
            Joystick.hpp  
	    LogitechG27.hpp  
	    SliderBox.hpp
            ConnexionHID.hpp
            GraupnerMC20.hpp
    DEPS_PKGCONFIG  base-types iodrivers_base libsysfs libusb)



rock_executable(JoystickTest JoystickTest.cpp
    DEPS controldev)

rock_executable(ConnexionTest connexion_test.cpp
    DEPS controldev)

rock_executable(LogitechG27Init LogitechG27Init.cpp
    DEPS controldev)

ADD_EXECUTABLE(LogitechG27Test LogitechG27Test.cpp)
TARGET_LINK_LIBRARIES(LogitechG27Test controldev pthread)

rock_executable(SliderboxTest testSB.cpp
    DEPS controldev)

