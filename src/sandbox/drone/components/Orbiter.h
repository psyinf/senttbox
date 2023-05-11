#pragma once
#include <entt/entt.hpp>

struct Orbiter{
    entt::entity orbit{entt::null};
    double       epoch;
};