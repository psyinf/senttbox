#pragma once
#include <components/RenderModel.h>
#include <cereal/types/string.hpp>
#include <serializers/serializers_gmtl_cereal.h>

template <typename Archive>
void serialize(Archive& archive, RenderModel& rm)
{
    archive(cereal::make_nvp("path", rm.path));
    archive(cereal::make_nvp("scale", rm.scale));
    archive(cereal::make_nvp("offset", rm.offset));
}