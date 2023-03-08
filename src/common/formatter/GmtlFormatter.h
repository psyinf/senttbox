#pragma once
#include <fmt/ostream.h>
#include <gmtl/gmtl.h>

template <>
struct fmt::formatter<gmtl::Vec3d> : fmt::formatter<double>
{
    //TODO: find a way to reuse precision and f/e-representation
    template <typename FormatContext>
    auto format(const gmtl::Vec3d& v, FormatContext& ctx) const -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "({}, {}, {})", v[0], v[1], v[2]);
    }
};
