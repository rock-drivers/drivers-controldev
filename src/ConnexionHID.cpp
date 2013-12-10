#include "ConnexionHID.hpp"

#include <linux/input.h>
#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define PATH_BUFFER_SIZE (1024)

namespace controldev{


ConnexionHID::ConnexionHID(){
   scale[TX] = 1;
   scale[TY] = 1;
   scale[TZ] = 1;
   scale[RX] = 1;
   scale[RY] = 1;
   scale[RZ] = 1;
   t_scale = 350;
   r_scale = 350;
   oldValues.setZero();
}

ConnexionHID::~ConnexionHID(){
    close();
}

double& ConnexionHID::axisScalinig(Mapping id){
    return scale[id];
}

int ConnexionHID::getFileDescriptor() {
    return fd;
}

bool ConnexionHID::init(){

  struct dirent *entry;
  DIR *dp;
  char path[PATH_BUFFER_SIZE];
  struct input_id device_info;
  const char *devDirectory = "/dev/input/";

  /* open the directory */
  dp = opendir(devDirectory);
  if(dp == NULL) {
    return false;
  }
  /* walk the directory (non-recursively) */
  while((entry = readdir(dp))) {
    strncpy(path, devDirectory, sizeof(path));
    /* if strlen(devDirectory) > sizeof(path) the path won't be NULL terminated
     * and *bad things* will happen. Therfore, we force NULL termination.
     */
    path[PATH_BUFFER_SIZE-1] = '\0';
    strncat(path, entry->d_name, sizeof(path));

    fd = open(path, O_RDONLY | O_NONBLOCK);
    if(-1 == fd) {
      /* could not open file. probably we do not have read permission. */
      continue;
    }

    /* try to read the vendor and device ID */
    if(!ioctl(fd, EVIOCGID, &device_info)) {
      
      //LOG_DEBUG("Spacemouse: vendor %04hx product %04hx version %04hx\n",
      //        device_info.vendor, device_info.product, device_info.version);
      if((device_info.vendor == LOGITECH_VENDOR_ID) && 
         (device_info.product == LOGITECH_SPACE_NAVIGATOR_DEVICE_ID)) {
        /* BINGO!!! this is it! */
        break;
      }
    }
    ::close(fd);
    fd = -1;
  }

  if(-1 == fd) {
    fprintf(stderr,"could not find SpaceMouse! \nDo you have read permission on the /dev/input/ device?\n");
    return false;
  }
  closedir(dp);

  struct input_absinfo abs;

  //Does not work
  if(ioctl (fd, EVIOCGABS (REL_X), &abs) == 0){
      t_scale = abs.maximum;
      printf("RelX Scale: %f\n",t_scale);
  }
  if(ioctl (fd, EVIOCGABS (REL_RX), &abs) == 0){
      r_scale = abs.maximum;
      printf("RelRX Scale: %f\n",r_scale);
  }

  if(ioctl (fd, EVIOCGABS (ABS_X), &abs) == 0){
      t_scale = abs.maximum;
      printf("Abs X Scale: %f\n",t_scale);
  }
  if(ioctl (fd, EVIOCGABS (ABS_RX), &abs) == 0){
      r_scale = abs.maximum;
      printf("Abs RX Scale: %f\n",r_scale);
  }
  

  return true;
}

void ConnexionHID::close() {
  if(fd > 0) {
    ::close(fd);
  }
  fd = -1;
}

void ConnexionHID::getValue(struct connexionValues &coordinates, struct connexionValues &rawValues) {
    assert(fd > 0);
    rawValues = oldValues;
  /* If input events don't come in fast enough a certain DoF may not be 
   * updated during a frame. This results in choppy and ugly animation.
   * To solve this we record the number of frames a certain DoF was idle
   * and only set the DoF to 0 if we reach a certain idleThreshold.
   * When there is activity on a axis the idleFrameCount is reset to 0.
   */
  int i, eventCnt;
  /* how many bytes were read */
  size_t bytesRead;
  /* the events (up to 64 at once) */
  struct input_event events[64];
  /* keep track of idle frames for each DoF for smoother animation. see above */
  static int idleFrameCount[6] = {0, 0, 0, 0, 0, 0};
  int idleThreshold = 3;

  /* read the raw event data from the device */
  bytesRead = read(fd, events, sizeof(struct input_event) * 64);
  eventCnt = (int) ((long)bytesRead / (long)sizeof(struct input_event));
  if (bytesRead < (int) sizeof(struct input_event)) {
    perror("evtest: short read");
    return;
  }

  /* Increase all idle counts. They are later reset if there is activity */
  for(i = 0; i < 6; ++i) {
    ++idleFrameCount[i];
  }

  /* handle input events sequentially */
  for(i = 0; i < eventCnt; ++i) {
    if(EV_KEY == events[i].type) {
      switch(events[i].code) {
      case BTN_0:
        rawValues.button1 = events[i].value;
        break;
      case BTN_1:
        rawValues.button2 = events[i].value;
        break;
      }
    } else if(EV_REL == events[i].type || EV_ABS == events[i].type) {
      switch(events[i].code) {
      //case ABS_X: //Same value as REL_* so because of the check above, this is not needed
      case REL_X:
        rawValues.tx = events[i].value/t_scale;
        idleFrameCount[0] = 0;
        break;
      //case ABS_Y:
      case REL_Y:
        rawValues.ty = events[i].value/t_scale;
        idleFrameCount[1] = 0;
        break;
      //case ABS_Z:
      case REL_Z:
        rawValues.tz = events[i].value/t_scale;
        idleFrameCount[2] = 0;
        break;
      //case ABS_RX:
      case REL_RX:
        rawValues.rx = events[i].value/t_scale;
        idleFrameCount[3] = 0;
        break;
      //case ABS_RY:
      case REL_RY:
        rawValues.ry = events[i].value/t_scale;
        idleFrameCount[4] = 0;
        break;
      //case ABS_RZ:
      case REL_RZ:
        rawValues.rz = events[i].value/t_scale;
        idleFrameCount[5] = 0;
        break;
      }
    }
  }

  /* Set rawValue to zero if DoF was idle for more than idleThreshold frames */
  for(i = 0; i < 6; ++i) {
    if(idleFrameCount[i] >= idleThreshold) {
      if(0==i) {
        rawValues.tx = 0;
      } else if (1==i) {
        rawValues.ty = 0;
      } else if (2==i) {
        rawValues.tz = 0;
      } else if (3==i) {
        rawValues.rx = 0;
      } else if (4==i) {
        rawValues.ry = 0;
      } else if (5==i) {
        rawValues.rz = 0;
      }
    }
  }

  coordinates.tx = rawValues.tx  * scale[TX];
  coordinates.tz = -rawValues.tz * scale[TY];
  coordinates.ty = -rawValues.ty * scale[TZ];
  coordinates.rx = rawValues.rx  * scale[RX];
  coordinates.rz = -rawValues.rz * scale[RY];
  coordinates.ry = -rawValues.ry * scale[RZ];
  coordinates.button1 = rawValues.button1;
  coordinates.button2 = rawValues.button2;
  oldValues= rawValues;
}


}
