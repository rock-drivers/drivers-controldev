#ifndef _CONNEXIONHID_H_
#define _CONNEXIONHID_H_

#define LOGITECH_VENDOR_ID          0x046d
#define LOGITECH_SPACE_TRAVELLER_DEVICE_ID 0xc623
#define LOGITECH_SPACE_PILOT_DEVICE_ID     0xc625
#define LOGITECH_SPACE_NAVIGATOR_DEVICE_ID 0xc626
#define LOGITECH_SPACE_EXPLORER_DEVICE_ID  0xc627

namespace controldev{

  struct connexionValues {
    double tx;
    double ty;
    double tz;
    double rx;
    double ry;
    double rz;
    int button1;
    int button2;
  
    void setZero(){
        tx=0;
        ty=0;
        tz=0;
        rx=0;
        rz=0;
        button1=false;
        button2=false;
    };
  };


class ConnexionHID{

public:
  enum Mapping{
    TX = 0,
    TY,
    TZ,
    RX,
    RY,
    RZ
  };
  
  ConnexionHID();
  ~ConnexionHID();

  /* Scan all devices in /dev/input/ to find the SpaceMouse.
   * Returns the true if an SpaceMouse could be found.
   */
  bool init();
  
  void close();

  /**
   * Returns the file Decriptor, -1 if no valid FD is there
   */
  int getFileDescriptor();

  /** 
   * Refrence getter for axis scalinig, modifications are allowed via this reference member
   */
  double& axisScalinig(Mapping id);

  void getValue(struct connexionValues &coordiantes, struct connexionValues &rawValues);

protected:
  int fd;
  double scale[6];
  connexionValues oldValues;
  double r_scale;
  double t_scale;
};

#endif

};
