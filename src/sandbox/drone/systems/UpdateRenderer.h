#pragma once
#include "System.h"
#include <components/Kinematic.h>
#include <components/RenderModel.h>
#include <components/OrbitalParameters.h>
#include <entt/entt.hpp>
#include <unordered_set>



class UpdateRenderer : public System
{
public:
    explicit UpdateRenderer(Scene& scene, UpdateQueue& q)
        : System(scene)
        , updateQueue(q)
    {
        // entt::observer observer{registry, entt::collector.update<sprite>()};
        scene.getRegistry().on_destroy<RenderModel>().connect<&UpdateRenderer::destroyed>(this);
    }

    void destroyed(entt::registry&, entt::entity e)
    {
        deleted_entities.insert(e);
    }

    void update(Scene& scene, const FrameStamp& frame_stamp) override
    {

        // TODO: use a lockfree queue?
        // TODO: diff/change based update
        UpdateQueue::Queue updates;
        {
            auto view = scene.getRegistry().view<RenderModel, Kinematic, StaticTransform>();
            for (auto& entity : view)
            {
                auto&& [transform, kin, model] = view.get<StaticTransform, Kinematic, RenderModel>(entity);
                model.scale                    = std::pow(kin.mass, 1.0 / 3.0);
                updates.emplace(entity, Update{transform, model});
            }
        }
     
        {
            auto view = scene.getRegistry().view<OrbitalParameters,RenderModel>();
            for (auto& entity : view)
            {
                auto&& [orbital, model] = view.get<OrbitalParameters, RenderModel>(entity);
                updates.emplace(entity, Update{StaticTransform(), model});
            }
        }
        for (auto entity : deleted_entities)
        {
            updates.emplace(entity, Update{{}, {}, {true}});
        }

        deleted_entities.clear();
        updateQueue.push(std::move(updates));
    }

private:
    UpdateQueue&                             updateQueue;
    mutable std::unordered_set<entt::entity> deleted_entities;
};