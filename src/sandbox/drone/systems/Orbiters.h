#pragma once
#include "System.h"

#include <components/OrbitalParameters.h>
#include <components/Orbiter.h>
#include <components/StaticTransform.h>
#include <core/Scene.h>
#include <execution>
#include <random>
/*
Update of transform based on orbital parameters
*/

class Orbiters : public System
{
    using System::System;

public:
    virtual ~Orbiters() = default;


    void update(Scene& scene, const FrameStamp& stamp) override
    {
        auto view = scene.getRegistry().view<StaticTransform, Orbiter>();
        for (auto&& [entity, spatial, orbiter] : view.each())
        {
            orbiter.epoch += 0.04;
            if (scene.getRegistry().valid(orbiter.orbit)) {
                auto orbital_parameters = scene.getRegistry().get<OrbitalParameters>(orbiter.orbit);
                spatial.position        = OrbitalMechanics::getEulerAngelsAtJulianDay(orbital_parameters, orbiter.epoch).toCartesian();
        
            }
        }
    }
};
