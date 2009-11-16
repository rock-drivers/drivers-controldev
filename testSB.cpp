#include "SliderBox.hpp"
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>

using namespace controldev;

int main () 
{
    SliderBox sl;
    
    if(!sl.init()) {
	std::cout << "Failed to open Sliderbox" << std::endl;
	exit(1);
    }
    
    if(!sl.connectBox()) {
	std::cout << "Failed to connect to Sliderbox" << std::endl;
	exit(1);
    }
  
    timeval tv1, tv2;
    timeval *tv_old = &tv1, *tv_new = &tv2;
    gettimeofday(&tv1, NULL);
    gettimeofday(&tv2, NULL);

    bool updated = false;
    
    while(true) {
	
	if(sl.pollNonBlocking(updated)) {
	    gettimeofday(tv_new, NULL);

	    long diff = (tv_new->tv_sec - tv_old->tv_sec)*1000000 + (tv_new->tv_usec - tv_old->tv_usec);
	    std::cout << "Usec " << tv_new->tv_usec << " " << tv_old->tv_usec << " ";
	    std::cout << "Got new Data diff is " << diff << " " << std::endl;
	    *tv_old = *tv_new;
	    if(updated)
		std::cout<<"Got Update" << std::endl;
	}

	
	if(sl.getFileDescriptor() == -1) {
	    std::cout << "Read Error" << std::endl;
	    exit(1);
	}
    

  }
  

    

}
