#pragma once
#include <gmtl/gmtl.h>
#include <nlohmann/json.hpp>

// serializers
namespace gmtl 
{
template<class DATA_TYPE, unsigned S>
inline void to_json(nlohmann::json& j, const VecBase<DATA_TYPE, S> & p)
{
    j = nlohmann::json(p.mData);
}
template <class DATA_TYPE, unsigned S>
inline void from_json(const nlohmann::json& j, VecBase<DATA_TYPE, S>& p)
{
    j.get_to(p.mData);
}

/* FIXME: Compiler error on GCC 12
inline void to_json(nlohmann::json& j, const Quatd& q)
{
    j = nlohmann::json::array({*    q.getData()});
}

inline void from_json(const nlohmann::json& j, Quatd& q)
{
    j.get_to(q.mData);
}
*/
template <typename DATA_TYPE, typename ROTATION_ORDER>
inline void to_json(nlohmann::json& j, const EulerAngle < DATA_TYPE, ROTATION_ORDER> &e)
{
    j = nlohmann::json(std::span(e.getData(),3));
}
template <typename DATA_TYPE, typename ROTATION_ORDER>
inline void from_json(const nlohmann::json& j, EulerAngle<DATA_TYPE, ROTATION_ORDER>& e)
{
    //TODO: eliminate copy by directly serializing into the span
    auto rot_data_span = std::span(e.getData(), 3);
    auto a = j.get<std::array<DATA_TYPE,3>>();
    rot_data_span[0]   = a[0];
    rot_data_span[1]   = a[1];
    rot_data_span[2]   = a[2];


    
}

}