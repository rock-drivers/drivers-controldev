
#include "Joystick.hpp"
#include "EvdevHelper.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/joystick.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sstream>
#include <stdexcept>
#include <limits>


namespace controldev
{

    Joystick::Joystick() : initialized(false), deadspot(false), deadspot_size(0)
    {
	axes = 0;
	buttons = 0;
	fd = -1;
    }

    Joystick::~Joystick()
    {
      if(axes)
	delete[] axes;

      if(buttons)
	delete[] buttons;

      if(fd != -1)
	close(fd);
	
      if(!initialized)
        return;
    }

    bool Joystick::init(std::string const& dev)
    {
        initialized = false;

        if(axes) {
        delete[] axes;
        axes = 0;
        }

        if(buttons) {
        delete[] buttons;
        buttons = 0;
        }

        if(fd != -1)
        close(fd);

        if ((fd = open(dev.c_str(),O_RDONLY | O_NONBLOCK)) < 0) {
        //std::cout << "Warning: could not initialize joystick.\n";
        fd = -1;
        return false;
        } 

        if(ioctl(fd, JSIOCGAXES, &nb_axes) == -1) {
        perror("axes");
        return false;
        }
        if(ioctl(fd, JSIOCGBUTTONS, &nb_buttons) == -1) {
        perror("button");
        return false;	
        }

        char name_char[50];

        if(ioctl(fd, JSIOCGNAME(50), name_char) == -1) {
        perror("name");
        return false;	
        }


        /** Get the axis mapping **/
        uint8_t axismap[ABS_MAX + 1];
        if (ioctl(fd, JSIOCGAXMAP, axismap) < 0)
        {
            std::ostringstream str;
            str << dev << ": " << strerror(errno);
            return false;
        }
        else
        {
            std::copy(axismap, axismap + nb_axes, std::back_inserter(this->axis_mapping));
        }

        /** Get the button mapping **/
        uint16_t btnmap[KEY_MAX - BTN_MISC + 1];
        if (ioctl(fd, JSIOCGBTNMAP, btnmap) < 0)
        {
            std::ostringstream str;
            str << dev << ": " << strerror(errno);
            return false;
        }
        else
        {
            std::copy(btnmap, btnmap + nb_buttons, std::back_inserter(this->button_mapping));
        }

        name = std::string(name_char);

        axes = new int[nb_axes];
        buttons = new int[nb_buttons];

        for(int i = 0; i < nb_buttons; i++) {
        buttons[i] = 0;
        }

        for(int i = 0; i < nb_axes; i++) {
        axes[i] = 0;
        }

        /** Generate and Display Joystick information **/
        std::cout<< "Joystick ("<< name <<") has "<< (int) nb_axes <<" Axes (";
        for(std::vector<int>::iterator i=this->axis_mapping.begin(); i!=this->axis_mapping.end(); ++i)
        {
            this->axis_names.push_back(abs2str(*i));
            std::cout<<abs2str(*i)<<" ";
        }
        std::cout<<")\n";

        std::cout << "It has "<< (int) nb_buttons << " Buttons (";
        for(std::vector<int>::iterator i=this->button_mapping.begin(); i!=this->button_mapping.end(); ++i)
        {
            this->button_names.push_back(btn2str(*i));
            std::cout<<btn2str(*i)<<" ";
        }
        std::cout<<")\n";
        /**  End Print **/


        initialized = true;

        return true;
    }


    void Joystick::setDeadspot(bool onOff, double size) {
      deadspot = onOff;
      deadspot_size = size;
    }

    bool Joystick::updateState()
    {
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
        axes[mybuffer[i].number] = mybuffer[i].value;

        if(mybuffer[i].number == 0 || mybuffer[i].number == 1) {
          if(deadspot) {
            if(abs(axes[mybuffer[i].number]) < deadspot_size * 32767)
              axes[mybuffer[i].number] = 0;
            else if(axes[mybuffer[i].number] > 0)
              axes[mybuffer[i].number] = (axes[mybuffer[i].number] - 
                          deadspot_size * 32767)
            / ((1.0-deadspot_size) * 32767.0) * 32767.0;
            else if(axes[mybuffer[i].number] < 0)
              axes[mybuffer[i].number] = 
            (axes[mybuffer[i].number] + 
             deadspot_size * 32767)
            / ((1.0-deadspot_size) * 32767.0) * 32767.0;
          } else
            axes[mybuffer[i].number] = axes[mybuffer[i].number];
        }
        if(mybuffer[i].number == 1 || mybuffer[i].number == 5)
          axes[mybuffer[i].number] *= -1;
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

    bool Joystick::getButtonPressed(int btn_nr) const
    {
      if(btn_nr > nb_buttons)
        return false;

      return buttons[btn_nr];
    }


    std::vector<bool> Joystick::getButtons() const
    {
      std::vector<bool> button_values;
      for (register int i=0; i<this->nb_buttons; ++i)
      {
          button_values.push_back(static_cast<bool>(buttons[i]));
      }
      return button_values;
    }


    double Joystick::getAxis(int axis_nr) const
    {
      if (!initialized) return std::numeric_limits<double>::quiet_NaN();
      if (axis_nr > nb_axes) return std::numeric_limits<double>::quiet_NaN();

      return axes[axis_nr] / 32767.0;
    }

    std::vector<double> Joystick::getAxes() const
    {
      std::vector<double> axis_values;
      if (!initialized) return axis_values;

      for (register int i=0; i<this->nb_axes; ++i)
          axis_values.push_back(static_cast<double>(axes[i]/32767.0));

      return axis_values;
    }


}
