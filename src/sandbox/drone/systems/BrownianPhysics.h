#pragma once
#include <components/Kinematic.h>
#include <core/Scene.h>
#include <random>
#include "System.h"
class BrownianPhysics : public System
{
private:

   
    
    std::random_device  rd{};
    mutable std::mt19937                         gen{rd()};
    

    mutable std::uniform_int_distribution<unsigned> int_dist{0u, 100u};
    mutable std::normal_distribution<> normal_dist{0, 0.5};
   
public:
    void update(Scene& scene, float timestamp) const override
    {
        auto view = scene.getRegistry().view<Kinematic>();
        
        for (auto&& [entity, kinematic] : view.each())
        {
            if (int_dist(gen) > 10)
            {
                kinematic.velocity +=
                    gmtl::Vec3d{
                        normal_dist(gen), normal_dist(gen), normal_dist(gen)};
            }
        }
    }
};
