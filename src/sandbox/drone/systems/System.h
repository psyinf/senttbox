#pragma once
#include <core/Update.h>
class System
{
public:
    virtual void update(Scene&, const FrameStamp& frame_stamp) const= 0;
};