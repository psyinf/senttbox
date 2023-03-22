#pragma once
#include "System.h"


class UpdateRenderer : public System
{
public:
    explicit 
UpdateRenderer(UpdateQueue& up)
        : updater(up)
    {
    }

    void update(Scene& scene, const FrameStamp& stamp) const override
    {
        //TODO: use a lockfree queue?
        UpdateQueue::Queue updates;
        auto view = scene.getRegistry().view<RenderModel, StaticTransform>();
        for (auto& entity : view)
        {
            auto&& [transform, model] = view.get<StaticTransform, RenderModel>(entity);
            
            updates.emplace(entity, Update{transform, model});
        }
        updater.push(std::move(updates));
    }

private:
    UpdateQueue& updater;
};