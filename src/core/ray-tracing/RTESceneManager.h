#pragma once


#include <embree4/rtcore_device.h>
#include <memory>
#include <mutex>

class RTESceneManager
{
private:
    std::unique_ptr<RTCScene> _topLevelScene;


};