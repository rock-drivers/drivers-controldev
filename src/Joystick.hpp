#ifndef JOYSTICK_HPP
#define JOYSTICK_HPP

#include <string>

namespace controldev
{
    class Joystick {
      public:
      Joystick();
      virtual ~Joystick();

      virtual bool init(std::string const& dev);
      
      void setDeadspot(bool onOff, double size);
      
      virtual bool updateState();
      
      virtual double getAxis(size_t axis_nr) const;

      virtual bool getButtonPressed(int btn_nr) const;

      virtual int getNrAxis() const {
        return nb_axes;
      }
      
      virtual int getNrButtons() const {
        return nb_buttons;
      }

      int getFileDescriptor()
      {
          return fd;
      }
    
      std::string const getName() const {
	  return name;
      }
      
      private:
      int fd;
      bool initialized;
      bool deadspot;
      double deadspot_size;
      
      int *axes;
      int *buttons;
      unsigned char nb_buttons;
      unsigned char nb_axes;
      std::string name;
        
    };
}     
#endif
