#ifndef JOYSTICK_HPP
#define JOYSTICK_HPP

#include <string>

namespace controldev
{
    class Joystick {
      public:
      Joystick();
      virtual ~Joystick();

      enum Axis
      {
          AXIS_Sideward = 0,
          AXIS_Forward = 1,
          AXIS_Turn = 2,
          AXIS_Slider = 3,
          AXIS_Pan = 4,
          AXIS_Tilt = 5, // What is the Tilt-Axis?
          AXIS_0 = 0,
          AXIS_1 = 1,
          AXIS_2 = 2,
          AXIS_3 = 3,
          AXIS_4 = 4,
          AXIS_5 = 5,
          AXIS_6 = 6,
          AXIS_7 = 7,
          AXIS_8 = 8
      };

//      static const int FORWARD_AXIS = 1;
//      static const int SIDEWARD_AXIS = 0;
//      static const int PAN_AXIS = 4;
//      static const int TILT_AXIS = 5;

      virtual bool init(std::string const& dev);
      
      void setDeadspot(bool onOff, double size);
      
      virtual bool updateState();
      
      virtual double getAxis(Axis axis_nr) const;

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
