//
// C++ Implementation: sliderbox
//
// Description: 
//
//
// Author: Janosch Machowinski <Janosch.Machowinski@dfki.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "SliderBox.hpp"
#include <sysfs/libsysfs.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

namespace controldev
{

const char SliderBox::transmission_on = 'n';
const char SliderBox::transmission_off = 'f';
const char SliderBox::alive = 'a';

SliderBox::SliderBox()
{
    initialized = false;
    connected = false;
}


SliderBox::~SliderBox()
{
}


bool SliderBox::connectBox() {
    connected = false;
    
    if(!initialized)
	return false;
    
    char buffer[1024];
    unsigned int error_count;
    int len;
    
    //send off
    if((len = write(serial_fd, &transmission_off, sizeof(char))) != sizeof(char)) {
	printf("len is %i \n", len);
	perror("1failed to write data to Sliderbox\n");
	return false;
    };
    
    //wait some time for box to answer
    usleep(100000);
    
    //read out all data
    while((len = read(serial_fd, buffer, 1024)) > 0) {
	printf(".");
    }
    
    if(len < 0 && errno != EAGAIN) {
	perror("failed to read data from ASGurad Sliderbox\n");
	return false;
    }
    
    //test if asguard box is alive
    if((len = write(serial_fd, &alive, sizeof(char))) != sizeof(char)) {
	perror("failed to write data to Sliderbox\n");
	return false;
    };
    
    error_count = 0;
    buffer[0] = 0;
    
    while(buffer[0] != alive) {
	//verify box answered
	while((len = read(serial_fd, buffer, sizeof(char))) < 0) {
	    //wait some time for box to answer
	    usleep(100000);
	    error_count++;
	    
	    if(error_count > max_read_errors) {
		printf("too many errors \n");
		return false;
	    }
	    
	    if(errno != EAGAIN)
		perror(".");
	    
	    printf("len is %i \n", len);
	}
    }
    
    printf("Got alive from Sliderbox\n");
    
    //tell box to start sending data
    if((len = write(serial_fd, &transmission_on, sizeof(char))) != sizeof(char)) {
	perror("failed to write data to Sliderbox\n");
	return false;
    };
    
    while(buffer[0] != transmission_on) {
	//verify box answered
	while((len = read(serial_fd, buffer, sizeof(char))) < 0) {
	    //wait some time for box to answer
	    usleep(100000);
	    error_count++;
	    
	    if(error_count > max_read_errors) {
		printf("too many errors \n");
		return false;
	    }
	    
	    if(errno != EAGAIN)
		perror(".");
	    
	    printf("len is %i \n", len);
	}
    }
    
    printf("Got transmission_on from Sliderbox\n");
    
    connected = true;
    
    return true;
}



/*!
\fn SliderBox::pollBlocking()
*/
bool SliderBox::pollNonBlocking(bool &updated)
{
    updated = false;
    if(!connected || !initialized)
	return false;
    
    static char buffer[1024];
    static unsigned int len = 0;
    
    int ret;
    
    ret = read(serial_fd, buffer + len, sizeof(struct sliderbox_data) - len);

    //bail out in case of error
    if(ret == -1 && errno != EAGAIN) 
    {
	initialized = false;
	connected = false;
	close(serial_fd);
	std::cout << "Error reading data from SliderBox" << std::endl;
	return false;
    }
    
    if(ret > 0)
	len += ret;
    
    //packet not fully received
    while(len >= sizeof(struct sliderbox_data)) {
	
	struct sliderbox_data *sl_data = (struct sliderbox_data *) buffer;
	
	if(sl_data->stop != end_bytes) {
	    printf("Got bad block from sliderbox");
	    len -= 1;
	    //copy down data
	    for(unsigned int i = 0; i < len; i++) {
		buffer[i] = buffer[i+1];
	    }
	    continue;
	}
	
	for(int i = 0; i < 7; i++) {
	    if(abs(old_slidervalues[i] - sl_data->slidervalues[i]) > 2) {
		old_slidervalues[i] = sl_data->slidervalues[i];
		updated = true;
	    }
	}
	
	for(int i = 0; i < 4; i++) {
	    if((sl_data->buttons & (1 << i)) != (buttons & (1 << i))) {
		updated = true;
	    }
	}
	
	buttons = sl_data->buttons;
	
	//mark packet as read
	len -=sizeof(struct sliderbox_data);
	
	//copy down data
	for(unsigned int i = 0; i < len; i++) {
	    buffer[i] = buffer[i+sizeof(struct sliderbox_data)];
	}
	
	//client should ask again for new update
	//there might be a second packet in the buffer
	return true;
    }
    
    //not enought data in buffer
    return false;
}



bool SliderBox::openDevice(std::string& path)
{
    serial_fd = open(path.c_str(), O_NONBLOCK | O_RDWR);
    
    if(serial_fd == -1) {
	perror("could not open Sliderbox device");
	return false;
    }
    
    struct termios termios_p;
    if(tcgetattr(serial_fd, &termios_p)){
	perror("Failed to get terminal info \n");
	return false;
    }
    
    cfmakeraw(&termios_p);
    
    if(cfsetispeed(&termios_p, B115200)){
	perror("Failed to set terminal input speed \n");
	return false;
    }
    
    if(cfsetospeed(&termios_p, B115200)){
	perror("Failed to set terminal output speed \n");
	return false;
    }
    
    if(tcsetattr(serial_fd, TCSANOW, &termios_p)) {
	perror("Failed to set speed \n");
	return false;
    }
    initialized = true;
    
    return true;
}


/*!
\fn SliderBox::init()
*/
bool SliderBox::init()
{
    initialized = false;
    struct sysfs_bus *usbbus = sysfs_open_bus("usb");
    if(usbbus == 0) {
	printf("Failed to open bus usb \n");
	return false;
    }
    struct dlist *usbdevices = sysfs_get_bus_devices(usbbus);
    if(!usbdevices) {
	printf("no usbdevices present \n");
	return false;
    }
    struct sysfs_attribute *curattr;
    
    dlist_for_each_data(usbdevices, curattr, struct sysfs_attribute) {
	if(curattr) {
	    if(!sysfs_path_is_dir(curattr->path)) {
		struct sysfs_device *curdev = sysfs_open_device_path(curattr->path);
		if(curdev) {
		    struct sysfs_attribute *interface = sysfs_get_device_attr(curdev, "interface");
		    if(interface) {
			if(interface->len == 17 && strcmp(interface->value, "AsguardSliderBox")) {
			    //printf("Product is %s \n", interface->value);
			    struct dlist *interfaceattrs = sysfs_open_directory_list(curdev->path);
			    struct sysfs_attribute *devname;
			    dlist_for_each_data(interfaceattrs, devname, struct sysfs_attribute) {
				//printf("Device Name is : %s %i \n", devname->name, devname->len);
				if(!strncmp(devname->name, "ttyUSB", 6)) {
				    char dev[1024];
				    dev[0] = '/';
				    dev[1] = 'd';
				    dev[2] = 'e';
				    dev[3] = 'v';
				    dev[4] = '/';
				    strcpy(dev + 5, devname->name);
				    printf("Found AsguardSliderBox at %s \n", dev);
				    
				    sysfs_close_device(curdev);
				    std::string path(dev);
				    return openDevice(path);			
				}
			    }
			}
		    }
		    sysfs_close_device(curdev);
		}
	    }
	}	
    }
    return false;
}
}
