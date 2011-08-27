#ifndef LOGITECHG27_HPP
#define LOGITECHG27_HPP

#include <string>
#include <stdint.h>
#include <usb.h>
#include <linux/usbdevice_fs.h>

// event thread
#include <pthread.h>

// signal
// #include <signal.h>

namespace controldev
{
    class LogitechG27 {
      public:
      LogitechG27();
      ~LogitechG27();

      enum Axis
      {
          AXIS_Wheel = 0,
          AXIS_Clutch = 1,
	  AXIS_Throttle = 2,
          AXIS_Brake = 3,
	  AXIS_Clutchdirleftright = 4,
          AXIS_Clutchdirupdown = 5, 
      };
      
      virtual bool init();
      
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
    
      std::string getEvDev() const {
	return evDev;
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
      char evDev[32];

//! g27 stuff
      
      long steerInitValue;
      
      struct usb_bus *busses;
      struct usb_bus *bus;
      struct usb_device *usbdev;
      ::usb_dev_handle *usb_handle;
      
      int *button_codes;
      int *axis_codes;

      int solveCode(int* list, int listsize, int code);      
      
      bool initProMode();
      bool initEvDev(char evDev[32]);
      bool openEvDev(char evDev[32]);
      
      virtual struct usb_device *  usb_find_device(uint16_t vendor,uint16_t product);
      virtual int send_command(char command[7], char command2[7]);
    };
}
#endif
