#pragma once
#include "System.h"

#include <components/Kinematic.h>
#include <core/Scene.h>
#include <execution>
#include <random>
class BrownianPhysics : public System
{
private:
    std::random_device   rd{};
    mutable std::mt19937 gen{rd()};


    mutable std::uniform_int_distribution<unsigned> int_dist{0u, 100u};
    mutable std::normal_distribution<>              normal_dist{0, 0.5};
    mutable std::vector<gmtl::Vec3d>                rands{100000};

public:
    BrownianPhysics(Scene& scene)
         : System(scene)
    {
        buildRand();
    }


    void buildRand() const
    {
        std::ranges::generate(rands, [this]() { return gmtl::Vec3d{
                                                    normal_dist(gen), normal_dist(gen), normal_dist(gen)}; });
    }

    void update(Scene& scene, const FrameStamp& stamp) override
    {
        if (stamp.frame_number > 5)
        {
            return;
        }
        auto view = scene.getRegistry().view<Kinematic>();

        std::for_each(std::execution::par_unseq, view.begin(), view.end(), [this,&stamp, &view](auto entity) {
            auto&& kinematic = view.get<Kinematic>(entity);
            if (int_dist(gen) > 90)
            {
                kinematic.velocity += rands[(entt::entt_traits<entt::entity>::to_integral(entity) + stamp.frame_number) % rands.size()];
            }
        });
    }
};
