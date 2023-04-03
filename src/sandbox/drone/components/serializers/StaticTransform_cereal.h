#pragma once
#include <components/StaticTransform.h>
#include <cereal/types/string.hpp>
#include <serializers/serializers_gmtl_cereal.h>

template <typename Archive>
void serialize(Archive& archive, StaticTransform& t)
{
    archive(cereal::make_nvp("pos", t.position));
    archive(cereal::make_nvp("ori", t.orientation));
}