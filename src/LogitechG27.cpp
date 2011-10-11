#include "LogitechG27.hpp"
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

#include <stdbool.h>
#include <getopt.h>
#include <unistd.h>
#include <usb.h>
#include <time.h>
#include <linux/usbdevice_fs.h>
#include <stdint.h>
#include <string.h>

#include <sys/types.h>
#include <asm/types.h>

#include <linux/input.h>

// G27 usb ids
#define VENDOR 0x046d
#define G27NORMAL 0xc294
// #define G27EXTENDED 0xc299
#define G27PRO 0xc29b

// just needed for testing in openEvDev(char evDev[32])
#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)

// added for G27 usb
struct usb_dev_handle {
	  int fd;

	  struct usb_bus *bus;
	  struct usb_device *device;

	  int config;
	  int interface;
	  int altsetting;
	  void *impl_info;
};

namespace controldev
{
    enum SteerControlTypes {
        TYPE_SYN,
        TYPE_KEY,
        TYPE_RELATIVE,
        TYPE_ABSOLUTE
    };

   LogitechG27::LogitechG27() : initialized(false), deadspot(false), deadspot_size(0) {
	axes = 0;
	buttons = 0;
	fd = -1;
    }

  bool LogitechG27::init() {

    initialized = false;
    nb_axes = 0;
    nb_buttons = 0;
    axis_codes = new int[8];
    axes_inits = new int[8];
    button_codes = new int[32];
    
    if (!initProMode()) return false;
    if (!initEvDev(evDev)) return false;
    if (!openEvDev(evDev)) return false;
    
    if ((fd = open(evDev, O_RDONLY)) < 0) {
	  printf("Couldn't open g27 filedescriptor !\n");
	  return false;
    }
    
    std::cout << "Found device " << name << " featuring " << (int) nb_axes << " axes, " << (int) nb_buttons << " buttons." << std::endl;
    
    axes = new int[nb_axes];
    buttons = new int[nb_buttons];
    
    for(int i = 0; i < nb_buttons; i++) {
      buttons[i] = 0;
    }

    for(int i = 0; i < nb_axes; i++) {
      // axes[i] = 0;
      axes[i] = axes_inits[i];
    }

    initialized = true;
    return true;
  }

  bool LogitechG27::initProMode()
  {
    //! look for Logitech G27 controller
      // search G27 in normal mode and set to pro mode
      usbdev=usb_find_device(VENDOR,G27NORMAL);
      if ( usbdev != NULL ) {
	printf ("found G27 in normal mode, setting to pro mode\n");
	char setextended[] = { 0xf8, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00 };
	char setextended2[] = { 0xf8, 0x09, 0x04, 0x01, 0x00, 0x00, 0x00 };
	send_command(setextended, setextended2);
	// wait a sec for the wheel to init
	sleep(1);
      }
      // no normal mode G27 found, look for already initialized device
      else {
	usbdev=usb_find_device(VENDOR,G27PRO);
	if ( usbdev != NULL ) {
	// found G27 in pro mode
	  printf ("Found G27 in pro mode.\n");
	}
	else {
	  printf ("Unable to find G27 device.\n");
	  return false;
	}
      }
      return true;
}


bool LogitechG27::initEvDev(char evDev[32])
{
  //! look for /dev/event*      
      bool found = false;
      int i = 0;
      std::string filename = "/dev/input/event";
      char fname[32];
      snprintf(fname, 32, "%s%d", filename.c_str(), i);
      struct stat stFileInfo;
      do {
	  printf ("Testing \"%s\"...", fname);
	  fd = open(fname, O_RDONLY);
	  printf ("returned %i\n", fd);
	  if (fd != -1) {
	      unsigned short id[4];
	      ioctl(fd, EVIOCGID, id);
	      if ((id[ID_VENDOR] == VENDOR) && (id[ID_PRODUCT] == G27PRO)) {
		found = true;
		printf ("Device found !\n");
	      }
	      close(fd);
	      if (found) break;
	  }
	  i++;
	  snprintf(fname, 32, "%s%d", filename.c_str(), i);
      } while (stat(fname,&stFileInfo) == 0);

      if (!found) return false;
      snprintf(evDev, 32, "%s%d", filename.c_str(), i);
      printf ("Using \"%s\"\n",fname);
      return true;
}

bool LogitechG27::openEvDev(char evDev[32])
{
    int version;
    unsigned short id[4];
    char devName[256] = "Unknown";

    if ((fd = open(evDev, O_RDONLY)) < 0) {
        printf("Couldn't open device %s.", evDev);
        return false;
    }

    if (ioctl(fd, EVIOCGVERSION, &version)) {
        printf("Couldn't get driver version.");
        return false;
    }
    
    printf("Input driver version is %d.%d.%d\n", version >> 16, (version >> 8) & 0xff, version & 0xff);

    ioctl(fd, EVIOCGID, id);
        printf("Input device ID: bus 0x%ix vendor 0x%ix product 0x%ix version 0x%ix\n",
		id[ID_BUS], 
                id[ID_VENDOR],
                id[ID_PRODUCT],
                id[ID_VERSION]
               );
	
        ioctl(fd, EVIOCGNAME(sizeof(devName)), devName);
        name = std::string(devName);
	
	printf("Input device name: \"%s\"\n", devName);

        int i, j, k;
        int abs[5];
        unsigned long bit[EV_MAX][NBITS(KEY_MAX)];

        memset(bit, 0, sizeof(bit));
        ioctl(fd, EVIOCGBIT(0, EV_MAX), bit[0]);
        printf("Supported events:\n");

        for (i = 0; i < EV_MAX; i++)
	if (test_bit(i, bit[0])) {
	  printf("Event type %i\n",i);
	  if (!i) continue;
	  ioctl(fd, EVIOCGBIT(i, KEY_MAX), bit[i]);
	  for (j = 0; j < KEY_MAX; j++)
	  if (test_bit(j, bit[i])) {
	    printf("	Event code %i\n",j);
	    if (i == EV_KEY) {
	      button_codes[nb_buttons] = j;
	      nb_buttons++;
	    }
	    if (i == EV_ABS) {
	      ioctl(fd, EVIOCGABS(j), abs);	      
	      axis_codes[nb_axes] = j;
	      axes_inits[nb_axes] = abs[0];
	      nb_axes++;
	      printf("min: %i, max: %i, current: %i\n", abs[1], abs[2], abs[0]);
	      /*
		for (k = 0; k < 5; k++)
		  if ((k < 3) || abs[k])
		    printf("%i ",abs[k]);
		  printf("\n");
	      */
	    }
	  }
	}
	close(fd);
	return true;
}

    void LogitechG27::setDeadspot(bool onOff, double size) {
      deadspot = onOff;
      deadspot_size = size;
    }

    bool LogitechG27::updateState() {
  
      int rd, i;
      struct input_event ev[64];
      rd = read(fd, ev, sizeof(struct input_event) * 64);
      if (rd != -1) {
	for (i = 0; i < rd / (signed int) sizeof(struct input_event); i++) {
	  if (ev[i].type == TYPE_KEY) {
	      buttons[solveCode(button_codes, nb_buttons, ev[i].code)] = ev[i].value;
	  } else if (ev[i].type == TYPE_ABSOLUTE) {
	    axes[solveCode(axis_codes, nb_axes, ev[i].code)] = ev[i].value;
	  }
	} 
      }
      return true;
    }
    
      bool LogitechG27::getButtonPressed(int btn_nr) const{

      if(btn_nr > nb_buttons)
        return false;
      
      return buttons[btn_nr];


/*
      int btn_nr = solveCode(button_codes, nb_buttons, btn_code);
      if (btn_nr < 0) return false;
      
      return buttons[btn_nr];
*/
    }
    
    int LogitechG27::solveCode(int* list, int listsize, int code)
    {
      for (int i = 0; i < listsize; i++) {
	if (list[i] == code) return i;
      }
      return -1;
    }

    struct usb_device *  LogitechG27::usb_find_device(uint16_t vendor,uint16_t product){
      usb_init();
      usb_find_busses();
      usb_find_devices();

      busses = usb_get_busses();
      for (bus = busses; bus; bus = bus->next) {

	    for (usbdev = bus->devices; usbdev; usbdev = usbdev->next) {
	      if (usbdev->descriptor.idVendor == vendor && usbdev->descriptor.idProduct == product) {
		return usbdev;
	      }
	    }
      }
      return NULL;
    }

    int LogitechG27::send_command(char command[7], char command2[7]) {

      int stat;

    usb_handle = usb_open(usbdev);
    if (!usb_handle) {
	fprintf(stderr,"usb_open: %s\n",strerror(errno));
      }
      printf("Device opened\n");
      stat = usb_detach_kernel_driver_np(usb_handle, 0);
      if (stat<0) {
	fprintf(stderr,"usb_detach_kernel_driver_np: %i %s\n",stat,strerror(-stat));
      }
      printf ("Kernel driver released\n");

    stat = usb_claim_interface(usb_handle, 0);
    if (stat<0) {
	if (stat == -EBUSY) fprintf(stderr, "usb_claim_interface: Check that you have permissions to write to %s/%s and, if you don't, that you set up udev correctly.\n", usb_handle->bus->dirname, usb_handle->device->filename);
	if (stat == -ENOMEM) fprintf(stderr, "usb_claim_interface: Insuffisant memory\n");
      }
      printf ("USB interface claimed\n");

      stat=usb_interrupt_write(usb_handle, 1, command, sizeof(command), 100);
      stat=usb_interrupt_write(usb_handle, 1, command2, sizeof(command2), 100);
      if ( (stat < 0) ) { 
	if (stat == -ENODEV) fprintf(stderr, "usb_interrupt_write: No such device, changed identity ?\n");
      }
      else {
	printf ("USB command sent\n");
	stat=usb_release_interface(usb_handle, 0);
	if (stat<0) {
	  fprintf(stderr,"usb_release_interface: %i %s\n",stat,strerror(-stat));
	}
	printf ("USB interface released\n");
      }

      stat=usb_close(usb_handle);
      if (stat<0) {
	     // USB_ERROR_STR("usb_close: %s\n",strerror(-stat));
      }

      stat=0; // usb_rebind_kernel_driver_np();
      if (stat<0) {
	fprintf(stderr,"usb_rebind_kernel_driver_np: %i %s\n",stat,strerror(-stat));
      }
     printf ("Kernel driver rebinded\n");

      return 0;
    }

    LogitechG27::~LogitechG27() {
      if(axes)
	delete[] axes;
      
      if(buttons)
	delete[] buttons;

      if(fd != -1)
	close(fd);
	
      if(!initialized)
        return;
    }

    double LogitechG27::getAxis(G27Axis axis_nr) const
    {
      if (!initialized) return 0;
      if (axis_nr > nb_axes) return 0;
      switch (axis_nr) {
	case AXIS_Wheel:
	  // is min: 0, max: 16383, we want [-1, 1]
	  return axes[axis_nr] / 8191.0 -1.0;
	  break;
	  
	case AXIS_Clutch:
	  return axes[axis_nr];
	  break;
	  
	case AXIS_Throttle:
	  return (255 - axes[axis_nr]) / 255.0;
	  break;
	  
	case AXIS_Brake:
	  return axes[axis_nr];
	  break;
	  
	case AXIS_Clutchdirleftright:
	  return axes[axis_nr];
	  break;
	  
	case AXIS_Clutchdirupdown:
	  return axes[axis_nr];
	  break;	
      }
//      return axes[axis_nr];
//	return axes[axis_nr] / 8191.0 -1.0;
    }



}
