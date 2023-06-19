#pragma once
#include "System.h"
#include <components/CentralBody.h>
#include <components/OrbitalParameters.h>
#include <components/Orbiter.h>
#include <components/SceneProperties.h>
#include <components/StaticTransform.h>
#include <core/Scene.h>
#include <execution>
#include <random>
#include <numbers>
/*
Update of transform based on orbital parameters
*/
namespace constants
{
    static const double G = 6.67430e-11; //> Newtons gravitational constant (m^3*kg^-1*s^-2)
}


class Orbiters : public System
{
    using System::System;

public:
    virtual ~Orbiters() = default;


    void update(Scene& scene, const FrameStamp& stamp) override
    {
        const auto& scene_props = scene.getRegistry().ctx().get<SceneProperties>();
        auto view = scene.getRegistry().view<StaticTransform, Orbiter, CentralBody>();
        for (auto&& [entity, spatial, orbiter, body] : view.each())
        {
           
            if (scene.getRegistry().valid(orbiter.orbit)) {
                auto [orbital_parameters, central_body_ref] = scene.getRegistry().get<OrbitalParameters, CentralBodyRef>(orbiter.orbit);
                auto central_body = scene.getRegistry().get<CentralBody>(central_body_ref.central_body);
                auto orbital_period                          = 2.0 * std::numbers::pi * std::sqrt(std::pow(orbital_parameters.semimajor_axis, 3.0) / (constants::G * (central_body.mass + body.mass)));
                orbiter.epoch += FrameStamp::toSeconds(stamp.frame_time) * scene_props.timestep_scale / orbital_period;
                spatial.position        = OrbitalMechanics::getEulerAngelsAtFraction(orbital_parameters, orbiter.epoch).toCartesian();
        
            }
        }
    }
};
