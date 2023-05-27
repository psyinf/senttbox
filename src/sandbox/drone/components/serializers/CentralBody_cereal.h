#pragma once
#include <components/CentralBody.h>
#include <cereal/types/string.hpp>
#include <serializers/serializers_gmtl_cereal.h>

template <typename Archive>
void serialize(Archive& archive, CentralBody& cb)
{
    archive(cereal::make_nvp("name", cb.name));
    archive(cereal::make_nvp("mass", cb.mass));
}

template <typename Archive>
void serialize(Archive& archive, CentralBodyRef& cbr)
{
    archive(cereal::make_nvp("ref", cbr.central_body));
}