#pragma once

class System
{
public:
    virtual void update(Scene&, float frametime) const= 0;
};