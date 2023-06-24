#pragma once
#include <entt/entt.hpp>

struct CentralBodyRef
{
    entt::entity central_body{entt::null};
};

struct CentralBody
{
    std::string name{"The Sun"};
    double      mass{1.989e30};
};