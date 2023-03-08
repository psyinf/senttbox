#pragma once

#include <chrono>

#include <gmtl/gmtl.h>

namespace common::math{

class NotImplementedException : public std::runtime_error
{
public:
    using runtime_error::runtime_error;
};


/**
 * representation of spatial information on an entity
 */
class Spatial 
{
public:
	gmtl::Vec3d position;
    gmtl::EulerAngleZXYd orientation;
};

/**
 * First order derivate of a {@code Spatial}.
 */
class Dynamic : public Spatial
{
public:
    void        derive(const Spatial& prev, Spatial current, std::chrono::seconds) {
        throw NotImplementedException("Not implemented");
    }
    gmtl::Vec3d velocity;
    gmtl::Vec3d rot_velocity;
};

/**
 * Second order derivate of a {@code Spatial}.
 */
class Kinematic : public Dynamic
{
public:
    void        derive(const Dynamic& prev, Dynamic current, std::chrono::seconds) {
        throw NotImplementedException("Not implemented");
    }
    gmtl::Vec3d acceleration;
    gmtl::Vec3d rot_acceleration;
};


} //namespace msge
