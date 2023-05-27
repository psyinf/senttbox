#pragma once


struct CentralBodyRef
{
    entt::entity central_body{entt::null};
};

struct CentralBody
{
    std::string name{"Some Sun"};
    double      mass{1.989e30};
};