#pragma once
#include "System.h"

#include <components/Kinematic.h>
#include <components/StaticTransform.h>
#include <core/Scene.h>
#include <execution>
#include <random>
/*
Update of transform based on Kinematics
*/

class Physics : public System
{
    using System::System;

public:
    virtual ~Physics() = default;


    void update(Scene& scene, const FrameStamp& stamp) override
    {
        auto view = scene.getRegistry().view<StaticTransform, Kinematic>();
        // for (auto&& [entity, spatial, kinematic] : view.each())

        std::for_each(std::execution::par_unseq, view.begin(), view.end(), [&view, &stamp](auto& entity) {
            auto& spatial   = view.get<StaticTransform>(entity);
            auto& kinematic = view.get<Kinematic>(entity);
            spatial.position += stamp.toSeconds(stamp.frame_time) * kinematic.velocity;
            // some dampening
            // kinematic.velocity *= 0.99;
        });
    }
};
