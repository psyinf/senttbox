#pragma once
#include <gmtl/gmtl.h>


struct StaticTransform
{
    gmtl::Vec3d          position;
    gmtl::EulerAngleZXYd orientation;
};