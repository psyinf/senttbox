#include "core/Scene.h"
#include "renderer/ViewerCore.h"

#include <systems/BrownianPhysics.h>
#include <systems/Gravitation.h>
#include <systems/Physics.h>
#include <systems/UpdateRenderer.h>

void setupScene(Scene& scene)
{
    auto               scenario = 1;
    std::random_device   rd{};
    std::mt19937 gen{rd()};


    std::uniform_int_distribution<unsigned> int_dist{0u, 100u};
    std::normal_distribution<double>              normal_dist{10, 1.5};
    std::normal_distribution<double>        normal_dist2{0, 2.5};
    /*
    scene.makeDrone<StaticTransform, RenderModel>({gmtl::Vec3d{1, 0, 0}, gmtl::EulerAngleZXYd{}}, {"cone"});
    scene.makeDrone<StaticTransform, RenderModel>({gmtl::Vec3d{0, 1, 0}, gmtl::EulerAngleZXYd{}}, {"cone"});
    scene.makeDrone<StaticTransform, RenderModel>({gmtl::Vec3d{0, 0, 1}, gmtl::EulerAngleZXYd{}}, {"cone"});
    scene.makeDrone<StaticTransform, RenderModel>({gmtl::Vec3d{-1, 0, 0}, gmtl::EulerAngleZXYd{}}, {"cone"});
    scene.makeDrone<StaticTransform, RenderModel>({gmtl::Vec3d{0, -1, 0}, gmtl::EulerAngleZXYd{}}, {"cone"});
    scene.makeDrone<StaticTransform, RenderModel>({gmtl::Vec3d{0, 0, -1}, gmtl::EulerAngleZXYd{}}, {"cone"});
    */
    if (scenario == 1)
    {
        for (int i = 1200; i-- > 0;)
        {
            const float radius = normal_dist2(gen) * 100.0;
            scene.makeDrone<StaticTransform, Kinematic, RenderModel>({gmtl::Vec3d{sin(i / 400.0) * radius, cos(i / 400.0) * radius, normal_dist2(gen)}, gmtl::EulerAngleZXYd{}}, {.mass = normal_dist(gen)}, {.path = "sphere", .offset{0, 0, 0}});
            // scene.makeDrone<>(StaticTransform{gmtl::Vec3d{0, 0, 0}, gmtl::EulerAngleZXYd{}}, Kinematic{}, RenderModel{.path = "sphere", .offset{0, 0, 5}});
        }
    }
    
    else if (scenario == 3)
    {
        scene.makeDrone<StaticTransform, Kinematic, RenderModel>({gmtl::Vec3d{-5, 0, 0}, gmtl::EulerAngleZXYd{}}, {.mass = 5.0}, {.path = "sphere", .offset{0, 0, 0}});
        scene.makeDrone<StaticTransform, Kinematic, RenderModel>({gmtl::Vec3d{5, 0, 0}, gmtl::EulerAngleZXYd{}}, {.mass = 5}, {.path = "sphere", .offset{0, 0, 0}});
    }
    
}


class Simulation
{
public:
    void load(std::string_view scene_desc)
    {

        viewer.setup(updateQueue);
        setupScene(scene);
        systems.emplace_back(std::make_shared<Physics>(scene));
       // systems.emplace_back(std::make_shared<BrownianPhysics>(scene));
        systems.emplace_back(std::make_shared<OrbitalSystem>(scene));
        systems.emplace_back(std::make_shared<UpdateRenderer>(scene, updateQueue));
    }

    void runSystems()
    {

        std::ranges::for_each(systems, [this](auto& sys) { sys->update(scene, FrameStamp{std::chrono::milliseconds{16}, frame_number}); });
    }

    void frame()
    {
        std::chrono::nanoseconds ms_systems{};
        std::chrono::nanoseconds ms_rendering{};
        bool                     advance = true;
        while (advance)
        {

            auto pre = std::chrono::high_resolution_clock::now();

            runSystems();
            auto post_sys = std::chrono::high_resolution_clock::now();

            advance          = viewer.frame();
            auto post_render = std::chrono::high_resolution_clock::now();

            auto sys_time    = post_sys - pre;
            auto render_time = post_render - post_sys;
            ms_systems += sys_time;
            ms_rendering += render_time;
            ++frame_number;
            if (frame_number % 10 == 0)
            {
                std::cout << "sys: " << std::chrono::duration_cast<std::chrono::milliseconds>(ms_systems / 10)
                          << "render:  " << std::chrono::duration_cast<std::chrono::milliseconds>(ms_rendering / 10)
                          << "total: " << std::chrono::duration_cast<std::chrono::milliseconds>((ms_systems + ms_rendering) / 10)
                          << std::endl;
                ms_rendering = std::chrono::nanoseconds{};
                ms_systems   = std::chrono::nanoseconds{};
            }
        }
    }


private:
    Scene                                scene;
    ViewerCore                           viewer;
    std::vector<std::shared_ptr<System>> systems;
    UpdateQueue                          updateQueue;
    std::uint64_t                        frame_number = 0;
};

int main(int argc, char** argv)
{

    Simulation sim;
    sim.load("some");
    sim.frame();


    return 0;
}