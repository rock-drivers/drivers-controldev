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

      enum G27Axis
      {
          AXIS_Wheel = 0,
          AXIS_Clutch = 1,
	  AXIS_Throttle = 2,
          AXIS_Brake = 3,
	  AXIS_Clutchdirleftright = 4,
          AXIS_Clutchdirupdown = 5, 
      };
 
      
      enum G27Button
      {
	BTN_ClutchBlackDown = 0,
        BTN_ClutchBlackLeft = 1,
        BTN_ClutchBlackRight = 2,
	BTN_ClutchBlackUp = 3,
	BTN_FlapRight = 4,
	BTN_FlapLeft = 5,
	BTN_WheelRightUpper = 6,
	BTN_WheelLeftUpper = 7,
	BTN_ClutchRedTwo = 8,
	BTN_ClutchRedThree = 9,
	BTN_ClutchRedFour = 10,
	BTN_ClutchRedOne = 11,
	BTN_ClutchGearFirst = 12,
	BTN_ClutchGearSecond = 13,
	BTN_ClutchGearThird = 14,
	BTN_ClutchGearFourth = 15,
	BTN_ClutchGearFifth = 16,
	BTN_ClutchGearSixth = 17,	
	BTN_WheelRightMiddle = 18,
	BTN_WheelRightLower = 19,
	BTN_WheelLeftMiddle = 20,
	BTN_WheelLeftLower = 21,
	BTN_ClutchGearReverse = 22
      };
      
/*      
      enum G27ButtonCodes
      {
	BTN_ClutchBlackDown = 288,
        BTN_ClutchBlackLeft = 289,
        BTN_ClutchBlackRight = 290,
	BTN_ClutchBlackUp = 291,
	BTN_FlapRight = 292,
	BTN_FlapLeft = 293,
	BTN_WheelRightUpper = 294,
	BTN_WheelLeftUpper = 295,
	BTN_ClutchRedTwo = 296,
	BTN_ClutchRedThree = 297,
	BTN_ClutchRedFour = 298,
	BTN_ClutchRedOne = 299,
	BTN_ClutchGearFirst = 300,
	BTN_ClutchGearSecond = 301,
	BTN_ClutchGearThird = 302,
	BTN_ClutchGearFourth = 303,
	BTN_ClutchGearFifth = 720,
	BTN_ClutchGearSixth = 721,	
	BTN_WheelRightMiddle = 722,
	BTN_WheelRightLower = 723,
	BTN_WheelLeftMiddle = 724,
	BTN_WheelLeftLower = 725,
	BTN_ClutchGearReverse = 726,
      };
*/      
      virtual bool init();
      
      void setDeadspot(bool onOff, double size);
      
      virtual bool updateState();
      
      virtual double getAxis(G27Axis axis_nr) const;

      virtual bool getButtonPressed(int btn_nr) const;

      virtual int getNrAxis() const {
        return nb_axes;
      }
      
      virtual int getNrButtons() const {
        return nb_buttons;
      }
      
      virtual int getButtonCode(int btn_nr) {
	  return (btn_nr > nb_buttons)?-1:button_codes[btn_nr];
      }      
      
      int getFileDescriptor() {
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
      
      long *axes;
      long *buttons;
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
      
      long *button_codes;
      long *axis_codes;
      long *axes_inits;

      int solveCode(long* list, int listsize, int code);      
      
      bool initProMode();
      bool initEvDev(char evDev[32]);
      bool openEvDev(char evDev[32]);
      
      virtual struct usb_device *  usb_find_device(uint16_t vendor,uint16_t product);
      virtual int send_command(char command[7], char command2[7]);
    };
}
#endif
