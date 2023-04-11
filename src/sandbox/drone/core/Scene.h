#pragma once
#include <entt/entt.hpp>

#include <components/StaticTransform.h>

class Scene
{
public:
    Scene() = default;
    template <typename Component, typename... Args>
    void addComponent(entt::entity id, Component&& t)
    {
        scene_registry.emplace<Component>(id, std::forward<Component>(t));
    }

    template <typename Component, typename... Args>
    void addComponent(entt::entity id, Component&& t, Args&&... args)
    {
        scene_registry.emplace<Component>(id, std::forward<Component>(t));
        addComponent(id, std::forward<Args>(args)...);     
    }

    //make an entity by providing a number of components with their respective constructors
    template <typename... Components> 
    auto makeEntity(Components&&... components)
    {
        auto id = scene_registry.create();
        addComponent(id, std::forward<Components>(components)...);
      
        return id;
    }

    template <typename... Components, typename Archive> 
    void save(const entt::registry& reg, Archive& archive) {
        entt::snapshot{reg}.entities(archive).component<Components...>(archive);
        //saveComponent<Components...>(archive, snapshot);       
    }

    template <typename... Components, typename Archive>
    void load(entt::registry& reg, Archive& archive)
    {
        reg.clear();
        entt::continuous_loader{reg}.entities(archive).component<Components...>(archive).orphans();
        
        //loadComponent<Components...>(reg, archive);
    }

    explicit operator entt::registry&() { return scene_registry; }
    explicit operator const entt::registry&() const { return scene_registry; }

    entt::registry& getRegistry() { return scene_registry; }
    const entt::registry& getRegistry() const { return scene_registry; }

private:
    entt::registry scene_registry;
};
