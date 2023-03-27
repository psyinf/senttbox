#pragma once
#include "System.h"
#include <components/Kinematic.h>
#include <entt/entt.hpp>
#include <unordered_set>



class UpdateRenderer : public System
{
public:
    explicit UpdateRenderer(Scene& scene, UpdateQueue& up)
        : System(scene)
        , updater(up)
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
        UpdateQueue::Queue updates;
        auto               view = scene.getRegistry().view<RenderModel, Kinematic, StaticTransform>();
        for (auto& entity : view)
        {
            auto&& [transform, kin, model] = view.get<StaticTransform, Kinematic, RenderModel>(entity);
            model.scale                    = std::pow(kin.mass, 1.0 / 3.0);
            updates.emplace(entity, Update{transform, model});
        }
        for (auto entity : deleted_entities)
        {
            updates.emplace(entity, Update{{}, {}, {true}});
        }
        deleted_entities.clear();
        updater.push(std::move(updates));
    }

private:
    UpdateQueue&                             updater;
    mutable std::unordered_set<entt::entity> deleted_entities;
};