
#include "Joystick.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/joystick.h>
#include <errno.h>
#include <stdio.h>
#include <stdexcept>
#include <cmath>

namespace controldev
{

Joystick::Joystick() : fd(-1), initialized(false), deadspot(false), deadspot_size(0) {
}

bool Joystick::init(std::string const& dev) {
    initialized = false;
    axes.clear();
    buttons.clear();
    
    if(fd != -1)
        close(fd);

    if ((fd = open(dev.c_str(),O_RDONLY | O_NONBLOCK)) < 0) {
        //std::cout << "Warning: could not initialize joystick.\n";
        fd = -1;
        return false;
    } 

    unsigned char nb_axes;
    
    if(ioctl(fd, JSIOCGAXES, &nb_axes) == -1) {
        perror("axes");
        return false;
    }
    
    unsigned char nb_buttons;

    if(ioctl(fd, JSIOCGBUTTONS, &nb_buttons) == -1) {
        perror("button");
        return false;	
    }
    
    char name_char[50];
    
    if(ioctl(fd, JSIOCGNAME(50), name_char) == -1) {
        perror("name");
        return false;	
    }
    
    name = std::string(name_char);

    std::cout << "Axes: " << (int) nb_axes <<" Buttons: " << (int) nb_buttons << " Name: " << name <<std::endl;

    axes.resize(nb_axes, 0.0);
    buttons.resize(nb_buttons, false);

    initialized = true;

    return true;
}

      
void Joystick::setDeadspot(bool onOff, double size) {
    deadspot = onOff;
    deadspot_size = size;
}

void Joystick::applyDeadSpot()
{
    for(size_t i = 0; i < axes.size() ; i++)
    {
        if(std::abs(axes[i]) < deadspot_size)
            axes[i] = 0;
        else if(axes[i] > 0)
        {
            axes[i] = (axes[i] - deadspot_size) / (1.0 - deadspot_size);
        }
        else if(axes[i] < 0)
            axes[i] = (axes[i] + deadspot_size) / (1.0 - deadspot_size);
    }
}

bool Joystick::updateState() {
    struct js_event mybuffer[64];
    int n, i;

    if (!initialized)
        return false;

    n = read (fd, mybuffer, sizeof(struct js_event) * 64);
    if (n != -1) {
        for(i = 0; i < n / (signed int)sizeof(struct js_event); i++) {
            if(mybuffer[i].type & JS_EVENT_BUTTON &~ JS_EVENT_INIT) {
                buttons[mybuffer[i].number] = mybuffer[i].value;
            }
            else if(mybuffer[i].type & JS_EVENT_AXIS &~ JS_EVENT_INIT) {
                axes[mybuffer[i].number] = mybuffer[i].value / 32767.0;        
            }
        }
        return true;
    }

    //go into error state on any error
    if(errno != EAGAIN) {
        initialized = false;
        
        if(fd != -1)
            close(fd);

        fd = -1;
    }
    
    return false;

}

bool Joystick::getButtonPressed(size_t btn_nr) const{
    if (!initialized) 
        throw std::runtime_error("Button requested, before initialized");
    
    if(btn_nr > buttons.size())
        throw std::out_of_range("Requested non existing button");
    
    return buttons[btn_nr];
}



Joystick::~Joystick() {
    if(fd != -1)
        close(fd);
    
    if(!initialized)
        return;
}

double Joystick::getAxis(size_t axis_nr) const
{
    if (!initialized) 
        throw std::runtime_error("Axis requested, before initialized");

    if(axis_nr > axes.size())
        throw std::out_of_range("Requested non existing axis");
    
    return axes[axis_nr];
}

}
