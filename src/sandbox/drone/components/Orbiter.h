#pragma once
#include <entt/entt.hpp>

struct Orbiter{
    entt::entity orbit{entt::null};
    double       epoch; //> epoch as fraction of the orbital period of the referenced orbit
};