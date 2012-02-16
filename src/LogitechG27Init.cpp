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

int main(int argc, char**argv) {

  LogitechG27 *js = new LogitechG27();
  if (!js->init()) exit(1);
  
  if (js->updateState()) {
   printf("O.k.\n"); 
  }
      
}
