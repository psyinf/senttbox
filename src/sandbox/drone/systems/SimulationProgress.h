#pragma once
#include "System.h"

#include <components/SimulationState.h>
#include <components/SceneProperties.h>

#include <core/Scene.h>
#include <execution>
#include <random>
class SimulationProgress : public System
{
  

public:
    using System::System;

    void update(Scene& scene, const FrameStamp& stamp) override
    {
        auto& scene_props = scene.getRegistry().ctx().get<SceneProperties>();
        auto& sim_state   = scene.getRegistry().ctx().get<SimulationState>();
        if (!scene_props.paused)
		{
            sim_state.time.add(stamp.frame_time, scene_props.timestep_scale);
        }	

        
    }
};
