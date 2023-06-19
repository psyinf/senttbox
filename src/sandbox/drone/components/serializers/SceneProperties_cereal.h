#pragma once

#include <components/SceneProperties.h>


template <typename Archive>
void serialize(Archive& archive, SceneProperties& sp)
{
    archive(cereal::make_nvp("timestep_scale", sp.timestep_scale));
}