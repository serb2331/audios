#pragma once

#include <embree4/rtcore.h>

class RTEFacade {
private:
  RTCDevice _device;

public:
  RTEFacade();

  ~RTEFacade();
};