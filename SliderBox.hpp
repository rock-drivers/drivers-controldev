//
// C++ Interface: sliderbox
//
// Description: 
//
//
// Author: Janosch Machowinski <Janosch.Machowinski@dfki.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SLIDERBOX_H
#define SLIDERBOX_H

namespace controldev
{

    /**
        @author Janosch Machowinski <Janosch.Machowinski@dfki.de>
    */
    class SliderBox
    {
    public:
        static const char alive;
        static const char transmission_on;
        static const char transmission_off;
        static const unsigned short end_bytes = 0xffff;
        static const unsigned int max_read_errors = 10;
        
        SliderBox();

        ~SliderBox();
        bool init();
        bool connectBox();
        
        bool pollNonBlocking();

        unsigned short getValue(unsigned int nr) {
          if(nr > 6)
        return 0;

          return old_slidervalues[nr];
        }

        bool getButtonOn(unsigned int nr) {
          if(nr > 4)
        return false;
          return buttons & (1 << nr);
        }

        int getFileDescriptor()
        {
            return serial_fd;
        }

    private:
        int serial_fd;
        bool initialized;
        bool connected;
        unsigned short old_slidervalues[7];
        unsigned char buttons;

        struct sliderbox_data {
        unsigned short slidervalues[7];
          unsigned char buttons;
            unsigned short stop;
        } __attribute__ ((packed));
    };
}
#endif
