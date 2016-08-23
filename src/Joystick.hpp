#ifndef JOYSTICK_HPP
#define JOYSTICK_HPP

#include <string>
#include <vector>

namespace controldev
{
class Joystick {
public:
    Joystick();
    virtual ~Joystick();

    virtual bool init(std::string const& dev);
    
    void setDeadspot(bool onOff, double size);
    
    virtual bool updateState();
    
    virtual double getAxis(size_t axis_nr) const;

    virtual bool getButtonPressed(size_t btn_nr) const;

    virtual size_t getNrAxis() const {
    return axes.size();
    }
    
    virtual size_t getNrButtons() const {
    return buttons.size();
    }

    int getFileDescriptor()
    {
        return fd;
    }

    std::string const getName() const {
        return name;
    }
    
    const std::vector<double> &getAxes() const
    {
        return axes;
    };
    const std::vector<bool> &getButtons() const
    {
        return buttons;
    }
    
private:
    
    void applyDeadSpot();
    
    int fd;
    bool initialized;
    bool deadspot;
    double deadspot_size;

    std::vector<double> axes;
    std::vector<bool> buttons;
    std::string name;
    
};
}     
#endif
