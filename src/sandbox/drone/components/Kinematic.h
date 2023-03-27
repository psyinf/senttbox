#pragma once
#include <gmtl/gmtl.h>

struct Kinematic
{
    gmtl::Vec3d velocity;
    gmtl::Vec3d rot_velocity;
    double      mass{1.0};
};