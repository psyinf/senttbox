#pragma once
#include <core/Update.h>
class System
{
    Scene& scene;

public:
    System(Scene& scene)
    :scene(scene){

    }
    

    virtual void update(Scene&, const FrameStamp& frame_stamp) = 0;
};