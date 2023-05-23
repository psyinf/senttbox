#pragma once
#include <components/OrbitalParameters.h>
#include <cereal/types/string.hpp>
#include <serializers/serializers_gmtl_cereal.h>

template <typename Archive>
void serialize(Archive& archive, OrbitalParameters& op)
{
    archive(cereal::make_nvp("ecc", op.eccentricity));
    archive(cereal::make_nvp("semi-major", op.semimajor_axis));
    archive(cereal::make_nvp("inclination", op.incliniation));
    archive(cereal::make_nvp("arg-periapsis", op.longPA));
    archive(cereal::make_nvp("long-asc-node", op.longAN));
    archive(cereal::make_nvp("mean-longitude", op.meanLongitude));
}