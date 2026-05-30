#include "./RTEFacade.h"
#include <embree4/rtcore_device.h>

RTEFacade::RTEFacade() { _device = rtcNewDevice(NULL); }

RTEFacade::~RTEFacade() { rtcReleaseDevice(_device); }