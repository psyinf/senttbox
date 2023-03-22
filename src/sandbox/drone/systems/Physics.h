#pragma once
#include <components/Kinematic.h>
#include <components/StaticTransform.h>

#include <core/Scene.h>
#include "System.h"
#include <random>
#include <execution>
    /*
Update of transform based on Kinematics
*/

class Physics : public System
{
public:
    virtual ~Physics() = default;


    void update(Scene& scene, const FrameStamp& stamp) const override
    {
        auto view = scene.getRegistry().view<StaticTransform, Kinematic>();
        // for (auto&& [entity, spatial, kinematic] : view.each())
        
        std::for_each(std::execution::par_unseq, view.begin(), view.end(), [&view, &stamp](auto& entity) {
            auto& spatial = view.get<StaticTransform>(entity);
            auto& kinematic = view.get<Kinematic>(entity);
            spatial.position += stamp.toSeconds(stamp.frame_time) * kinematic.velocity;
            // some dampening
            kinematic.velocity *= 0.99;
        });
    }
};
