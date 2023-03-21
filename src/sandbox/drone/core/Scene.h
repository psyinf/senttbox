#pragma once
#include <entt/entt.hpp>

#include <components/StaticTransform.h>

class Scene
{
public:
    Scene() = default;
    template <typename T, typename... Args>
    void addComponent(entt::entity id, T t)
    {
        scene_registry.emplace<T>(id, t);

    }
    template <typename T, typename... Args>
    void addComponent(entt::entity id, T t, Args... args) {
        scene_registry.emplace<T>(id, t);
        addComponent(id, args...);     
    }
    template <typename... Args> 
    auto makeDrone(Args ... args) {
        auto id = scene_registry.create();
        addComponent(id, args...);
        //scene_registry.emplace<StaticTransform>(id, (args...));
        //scene_registry.emplace<RenderModel>(id, args...);
        return id;
    }
   
    

    explicit operator entt::registry&() { return scene_registry; }
    explicit operator const entt::registry&() const { return scene_registry; }

    entt::registry& getRegistry() { return scene_registry; }
    const entt::registry& getRegistry() const { return scene_registry; }

private:
    entt::registry scene_registry;
};
