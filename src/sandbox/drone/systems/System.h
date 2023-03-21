#pragma once

class System
{
    virtual void update(Scene&, float frametime) const= 0;
};