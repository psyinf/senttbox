#include "DeadReckoning.h"




common::math::Spatial common::math::DeadReckoning::extrapolateRPW(const Kinematic& s, std::chrono::milliseconds delta_t)
{
    auto    t = toSeconds(delta_t);
    Spatial result = extrapolateFPW(s, delta_t);
    
    gmtl::Vec3d          new_orientation  = s.rot_velocity * t;
    gmtl::EulerAngleZXYd current_rotation = s.orientation;
    current_rotation[0] += new_orientation[0];
    current_rotation[1] += new_orientation[1];
    current_rotation[2] += new_orientation[2];
    result.orientation = current_rotation;
    return result;
}

common::math::Spatial common::math::DeadReckoning::extrapolateRVW(const Kinematic& s, std::chrono::milliseconds delta_t)
{
    auto    t = toSeconds(delta_t);
    //TODO: check if the result should be used in further calculations
    Spatial result = extrapolateFVW(s, delta_t);
   
    gmtl::Vec3d          new_orientation  = s.rot_velocity * t;
    gmtl::EulerAngleZXYd current_rotation = s.orientation;
    current_rotation[0] += new_orientation[0];
    current_rotation[1] += new_orientation[1];
    current_rotation[2] += new_orientation[2];
    result.orientation = current_rotation;
    return result;
}


    

common::math::Spatial common::math::DeadReckoning::extrapolateFVW(const Kinematic& s, std::chrono::milliseconds delta_t)
{
    
    Spatial result(s);
    auto    t = toSeconds(delta_t);

    // P = P0 + t*V0 + t^2*0.5*a;
    result.position += gmtl::Vec3d(s.position * t) + s.acceleration * gmtl::Math::sqr(t);
    return result;
}

common::math::Spatial common::math::DeadReckoning::extrapolateFPW(const Dynamic& s, std::chrono::milliseconds delta_t)
{
    Spatial  result(s);
    auto          t       = toSeconds(delta_t);
    gmtl::Point3d old_pos = s.position;
    gmtl::Vec3d   old_vel = s.velocity;

    // P = P0 + t*V0;
    result.position = (s.position + gmtl::Vec3d(old_vel * t));
    return result;
}
//TODO: template function
// common::math::Spatial common::math::DeadReckoning::extrapolate(const Spatial& s, std::chrono::milliseconds delta_t, Type type /*= DRM_FPW*/)
// {
//     switch (type)
//     {
//     default:
//     case Type::FPW:
//         return extrapolateFPW(dynamic_cast<const Dynamic&>(s), delta_t);
//         break;
//     case Type::FVW:
//         return extrapolateFVW(s, delta_t);
//         break;
//     case Type::RPW:
//         return extrapolateRPW(s, delta_t);
//         break;
//     case Type::RVW:
//         return extrapolateRVW(s, delta_t);
//         break;
//     }
// }
