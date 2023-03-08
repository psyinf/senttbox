#pragma once

#include <strings/FixedLengthString.h>
#include <nlohmann/json.hpp>
// serializers
namespace common
{
template <size_t LENGTH>
inline void to_json(nlohmann::json& j, const FixedLengthString<LENGTH>& p)
{
    j = nlohmann::json(std::string_view(p));
}
template <size_t LENGTH>
inline void from_json(const nlohmann::json& j, FixedLengthString<LENGTH>& p)
{
    throw std::runtime_error("Not yet implemented");
    //j.get();
}


} // namespace common