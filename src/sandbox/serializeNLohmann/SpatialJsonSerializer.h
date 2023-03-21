#pragma once
#include <GmtlJsonSerializer.h>
#include <math/Spatial.h>
#include <nlohmann/json.hpp>

namespace common::math
{

inline void to_json(nlohmann::json& j, const Spatial& s)
{
    j = nlohmann::json{{"pos", s.position}, {"rot", s.orientation}};
}

inline void from_json(const nlohmann::json& j, Spatial& s)
{
    j.at("pos").get_to(s.position);
    j.at("rot").get_to(s.orientation);
}

inline void to_json(nlohmann::json& j, const Dynamic& s)
{

    nlohmann::to_json(j, static_cast<const Spatial&>(s));
    j.update({{"vel", s.velocity}, {"rotvel", s.rot_velocity}});
}

inline void from_json(const nlohmann::json& j, Dynamic& s)
{
    nlohmann::from_json(j, static_cast<Spatial&>(s));
    j.at("vel").get_to(s.velocity);
    j.at("rotvel").get_to(s.rot_velocity);
}
} // namespace msge