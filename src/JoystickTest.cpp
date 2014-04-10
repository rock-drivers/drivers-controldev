#include "Joystick.hpp"
#include "JoyPad.hpp"
#include <iostream>
#include <stdlib.h>

using namespace controldev;

int main(int argc, char**argv)
{

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

    /** Get Axes and Buttons names information **/
    std::vector<std::string> bnames = js->getMapButtons();
    std::vector<std::string> anames = js->getMapAxis();

    /** Update information **/
    while(1) {
	if(js->updateState())
        {
            register size_t k;

            /** Axes **/
            k = 0;
            std::vector<double> axis_values = js->getAxes();
            std::cout<<"Axis: ";
            for(std::vector<std::string>::iterator i=anames.begin(); i!=anames.end(); ++i)
            {
                std::cout<<(*i)<<": "<<axis_values[k]<<" ";
                k++;
            }
	    std::cout << std::endl;

            /** Buttons **/
            k = 0;
            std::vector<bool> button_values = js->getButtons();
            std::cout<<"Buttons: ";
            for(std::vector<std::string>::iterator i=bnames.begin(); i!=bnames.end(); ++i)
            {
                std::cout<<(*i)<<": "<<button_values[k]<<" ";
                k++;
            }
	    std::cout << std::endl;
            std::cout << "****              ****" <<std::endl;
	}
	
    }
}
