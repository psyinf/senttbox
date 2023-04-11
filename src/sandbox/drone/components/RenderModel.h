#pragma once
#include <string>
#include <gmtl/gmtl.h>
struct RenderModel
{
    std::string path;
    gmtl::Vec3f offset;
    double      scale{1.0};

};