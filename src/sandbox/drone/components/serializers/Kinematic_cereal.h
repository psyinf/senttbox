#pragma once
#include <components/Kinematic.h>
#include <cereal/types/string.hpp>
#include <serializers/serializers_gmtl_cereal.h>

template <typename Archive>
void serialize(Archive& archive, Kinematic& t)
{
    archive(cereal::make_nvp("vel", t.velocity));
    archive(cereal::make_nvp("rvel", t.rot_velocity));
    archive(cereal::make_nvp("mass", t.mass));
}