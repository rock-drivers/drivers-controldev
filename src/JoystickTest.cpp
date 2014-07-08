#include "Joystick.hpp"
#include "JoyPad.hpp"
#include <iostream>
#include <stdlib.h>

using namespace controldev;

int main(int argc, char**argv) {
    
    if(argc != 2) {
	std::cout << "Usage: testJoystick device" << std::endl;
	exit(0);
    }

    Joystick *js = new Joystick();
    js->init(argv[1]);
    
    if(js->getName() == std::string("Logitech Logitech RumblePad 2 USB")) {
	std::cout << "Detected Rumblepad" << std::endl;
	delete js;
	js = new JoyPad();
	js->init(argv[1]);
    }
    
    while(1) {
	if(js->updateState()) {
	    std::cout << "Got Update" << std::endl;
	    std::cout << "Forward Axis: " << js->getAxis(Joystick::AXIS_0) << std::endl;
	    std::cout << "Sideward Axis: " << js->getAxis(Joystick::AXIS_1) << std::endl;
	    std::cout << "Slider Axis: " << js->getAxis(Joystick::AXIS_2) << std::endl;
	    std::cout << "Buttons: ";
	    for(int i = 0; i < js->getNrButtons(); i++) {
		std::cout << js->getButtonPressed(i) << " ";
	    }
	    std::cout << std::endl;
	}
	
    }
    
    
    
    
    
    
    
    
    
}
