#include <components/Kinematic.h>
#include <components/StaticTransform.h>

class OrbitalSystem : public System
{
public:
    using System::System;

    void update(Scene& scene, const FrameStamp& frame_stamp) override
    {
        auto view = scene.getRegistry().view<StaticTransform, Kinematic>();

        for (auto entity : view)
        {
            auto& position = view.get<StaticTransform>(entity).position;
            auto& velocity = view.get<Kinematic>(entity).velocity;
            auto& mass     = view.get<Kinematic>(entity).mass;


            // Calculate the net gravitational force on the entity
            gmtl::Vec3d net_force = gmtl::Vec3d{};
            for (auto other_entity : view)
            {
                if (other_entity != entity)
                {
                    const auto& other_position = view.get<StaticTransform>(other_entity).position;
                    const auto& other_velocity = view.get<Kinematic>(other_entity).velocity;
                    const auto& other_mass     = view.get<Kinematic>(other_entity).mass;

                    gmtl::Vec3d displacement     = other_position - position;
                    double      distance_squared = gmtl::lengthSquared(displacement);
                    double      epsilon          = std::pow(mass, 1.0 /  3.0); // Set a small epsilon value

                    if (distance_squared > epsilon && distance_squared < 10000.0)
                    {
                        double distance = std::sqrt(distance_squared);

                        gmtl::Vec3d direction       = displacement / distance;
                        double      force_magnitude = (mass * other_mass) / distance_squared;
                        gmtl::Vec3d force           = direction * force_magnitude;
                        net_force += force;
                    }
                    if (distance_squared <= epsilon)
                    {
                        if (other_mass >= mass)
                        {
                            position = (other_position);
                        }
                        
                        auto e   = mass * 0.5 * gmtl::lengthSquared(velocity);
                        auto e2  = other_mass * 0.5 * gmtl::lengthSquared(other_velocity);
                        auto eg  = e + e2;
                        gmtl::Vec3d res_dir = (gmtl::makeNormal(velocity) * e) + (gmtl::makeNormal(other_velocity) * e2);
                        
                        mass += other_mass;

                        velocity     = gmtl::makeNormal(res_dir) * std::sqrt(2.0 * eg / mass);

                        
                        scene.getRegistry().destroy(other_entity);
                    }
                }
            }

            // Update the entity's velocity and position
            gmtl::Vec3d acceleration = net_force / mass;
            velocity += acceleration * frame_stamp.toSeconds(frame_stamp.frame_time);
            // position.value += velocity.value * dt;
        }
    }
};