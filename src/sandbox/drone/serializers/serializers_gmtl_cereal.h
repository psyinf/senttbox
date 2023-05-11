#pragma once
#include <gmtl/gmtl.h>
#include <span>

namespace gmtl
{

template <typename Archive, typename T, size_t SIZE>
void serialize(Archive& archive, gmtl::VecBase<T,SIZE>& vec)
{
    for (auto& x : std::span(vec.getData(), SIZE))
    {
        archive(x);
    }
}
template <typename Archive, typename T>
void serialize(Archive& archive, gmtl::VecBase<T, 3>& vec)
{
    archive(cereal::make_nvp("x", vec.mData[0]));
    archive(cereal::make_nvp("y", vec.mData[1]));
    archive(cereal::make_nvp("z", vec.mData[2]));
}
template <typename Archive, typename T, typename ROT_ORDER>
void serialize(Archive& archive, gmtl::EulerAngle<T,ROT_ORDER>& ori)
{
    archive(cereal::make_nvp("x", ori.getData()[0]));
    archive(cereal::make_nvp("y", ori.getData()[1]));
    archive(cereal::make_nvp("z", ori.getData()[2]));
}

}