#ifndef JOYPAD_H
#define JOYPAD_H

#include "Joystick.hpp"
#include <sys/time.h>

namespace controldev {

class JoyPad : public Joystick
{
    public:
	JoyPad();
	virtual double getAxis(int axis_nr) const;
	virtual bool getButtonPressed(int btn_nr) const;
	virtual int getNrAxis() const;
	virtual int getNrButtons() const;
	virtual bool init(const std::string& dev);
	virtual bool updateState();
    private:
	double sliderAxis;
	timeval lastUpdateTime;
};

}
#endif // JOYPAD_H
