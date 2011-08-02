#include "LogitechG27.hpp"
// #include "JoyPad.hpp"
#include <iostream>
#include <stdlib.h>

#include <stdio.h>
#include <usb.h>
#include <time.h>
#include <linux/usbdevice_fs.h>
#include <fcntl.h>

#include <linux/input.h>

using namespace controldev;

    enum SteerControlTypes {
        TYPE_SYN,
        TYPE_KEY,
        TYPE_RELATIVE,
        TYPE_ABSOLUTE
    };
    
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

void *g27EventHandler(void *arg)
{
  int df;
  printf ("connecting to %s .. ", (char*)arg);
  if ((df = open((char*)arg, O_RDONLY)) < 0) {
        printf("\nERROR (SteerControl Constructor): SteerControl was not found!\n");
        return (0);
  }
  printf("connected.\n");
  int rd, i;
  struct input_event ev[64];
  while( 1 ) {
    rd = read(df, ev, sizeof(struct input_event) * 64);
    if (rd < (int) sizeof(struct input_event)) {
      printf("Whoopsie. Mopo very sad.");
    }
    for (i = 0; i < rd / (signed int) sizeof(struct input_event); i++) {
      if (ev[i].type == EV_SYN) {
      } else if (ev[i].type == EV_MSC && (ev[i].code == MSC_RAW || ev[i].code == MSC_SCAN)) {
	printf("MISC_RAW_SCAN Event: time %i.%i, type %i ( ), code %i ( ), value %i\n",
		(int)ev[i].time.tv_sec,
		(int)ev[i].time.tv_usec,
		(int)ev[i].type,
		(int)ev[i].code,
		(int)ev[i].value);
      } else {
	printf("Event: time %i.%i, type %i ( ), code %i ( ), value %i\n", 
		(int)ev[i].time.tv_sec,
		(int)ev[i].time.tv_usec,
		(int)ev[i].type,
		(int)ev[i].code,
		(int)ev[i].value);
      }
      switch(ev[i].type) {
      case TYPE_SYN:
	  printf("syn");
	  break;
      case TYPE_KEY:
	  printf("key");
	  // emit steerButtonChanged(ev[i].code, ev[i].value);
	  break;
      case TYPE_RELATIVE:
	  printf("relative\n");
	  break;
      case TYPE_ABSOLUTE:
	  printf("abs\n");
	  // emit steerAxisChanged(ev[i].code, ev[i].value);
	  break;
      }
    }
  }
  return (0); 
}

int main(int argc, char**argv) {

  LogitechG27 *js = new LogitechG27();
  if (!js->init()) exit(1);
  
  if (js->updateState()) {
   printf("O.k.\n"); 
  }
  
  printf ("G27 detected at %s\n", js->getEvDev().c_str());
  pthread_t g27_event;
  pthread_create(&g27_event, NULL, g27EventHandler,(void*)js->getEvDev().c_str());
  pthread_join(g27_event, NULL);
    
}
