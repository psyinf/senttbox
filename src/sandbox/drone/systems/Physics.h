#pragma once
#include <components/Kinematic.h>
#include <components/StaticTransform.h>

#include <core/Scene.h>
#include "System.h"
#include <random>


class Physics : public System
{


public:
    void update(Scene& scene, float timestamp) const override
    {
        auto view = scene.getRegistry().view<StaticTransform, Kinematic>();

        for (auto&& [entity, spatial, kinematic] : view.each())
        {
            spatial.position += static_cast<double>(timestamp) * kinematic.velocity;            
            //some dampening
            kinematic.velocity *= 0.99;
        }
    }
};
