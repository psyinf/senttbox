#pragma once
#include <components/StaticTransform.h>
#include <components/RenderModel.h>
#include <unordered_map>
#include <entt/entt.hpp>

struct Update
{
    StaticTransform transform;
    RenderModel     model;
    
    //Update(Update&) = delete;

};

class UpdateQueue {
public:
    using Queue = std::unordered_map<entt::entity, Update>;

    void push(Queue&& b) {
        back = std::move(b);
    }

    void get(Queue& target) {
        back.swap(target);
    }
    Queue get()
    {
        Queue q;
        back.swap(q);
        return q;
    }

private:
    Queue back;
    
};
