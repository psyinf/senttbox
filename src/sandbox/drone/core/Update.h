#pragma once
#include <components/RenderModel.h>
#include <components/StaticTransform.h>
#include <entt/entt.hpp>
#include <unordered_map>


struct FrameStamp
{
    std::chrono::milliseconds frame_time{0};
    uint64_t                  frame_number{0};

    static double             toSeconds(std::chrono::milliseconds delta)
    {
        std::chrono::duration<double, std::ratio<1, 1>> t = delta;
        return t.count();
    }
};
struct Update
{
    StaticTransform transform;
    RenderModel     model;
    bool            remove = false;
};

class UpdateQueue
{
public:
    using Queue = std::unordered_map<entt::entity, Update>;

    void push(Queue&& b)
    {
        back = std::move(b);
    }

    void get(Queue& target)
    {
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
