#pragma once
#include <components/Orbiter.h>
#include <cereal/types/string.hpp>
#include <serializers/serializers_gmtl_cereal.h>

template <typename Archive>
void serialize(Archive& archive, Orbiter& o)
{
    archive(cereal::make_nvp("orbit", o.orbit));
    archive(cereal::make_nvp("epoch", o.epoch));

}