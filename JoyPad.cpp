#include "JoyPad.hpp"
#include <sys/time.h>
#include <iostream>

bool controldev::JoyPad::init(const std::string& dev)
{
    sliderAxis = 0.0;
    gettimeofday(&lastUpdateTime, NULL);
    return controldev::Joystick::init(dev);
}

bool controldev::JoyPad::updateState()
{
    
    bool ret = controldev::Joystick::updateState();
    
    timeval curTime;
    
    gettimeofday(&curTime, NULL);

    //diff in ms
    long diff = (curTime.tv_sec - lastUpdateTime.tv_sec)*1000 + (static_cast<int>(curTime.tv_usec) - lastUpdateTime.tv_usec) / 1000;

    lastUpdateTime = curTime;
    
    if(controldev::Joystick::getButtonPressed(5))
	sliderAxis += 0.005 * diff;
    if(controldev::Joystick::getButtonPressed(7))
	sliderAxis -= 0.005 * diff;
    
    if(sliderAxis > 1.0)
	sliderAxis = 1.0;
    
    if(sliderAxis < -1.0)
	sliderAxis = -1.0;
    
    return ret || controldev::Joystick::getButtonPressed(5) || controldev::Joystick::getButtonPressed(7);
}


double controldev::JoyPad::getAxis(controldev::Joystick::Axis axis_nr) const
{
    double val = 0;
    switch(axis_nr) {
	case AXIS_Sideward:
	    break;
	    
	case AXIS_Forward:
	    val = controldev::Joystick::getAxis(AXIS_Forward);
	    break;
	case AXIS_Turn:
	    val = controldev::Joystick::getAxis(AXIS_Turn);
	    break;
	case AXIS_Slider:
	    val = sliderAxis;
	    break;
    }
    return val;
}

bool controldev::JoyPad::getButtonPressed(int btn_nr) const
{
    if(btn_nr < 5)
      return controldev::Joystick::getButtonPressed(btn_nr);

    if(btn_nr == 5)
      return controldev::Joystick::getButtonPressed(btn_nr + 1);

    if(btn_nr > 5)
      return controldev::Joystick::getButtonPressed(btn_nr + 2);

    return false;
}

int controldev::JoyPad::getNrAxis() const
{
    return 4;
}

int controldev::JoyPad::getNrButtons() const
{
    return 8;
}

controldev::JoyPad::JoyPad()
{

}

