#include "core/Scene.h"
#include "renderer/ViewerCore.h"

#include <FactoryRegistry.h>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>
#include <components/serializers/Kinematic_cereal.h>
#include <components/serializers/RenderModel_cereal.h>
#include <components/serializers/StaticTransform_cereal.h>
#include <components/serializers/OrbitalParameters_cereal.h>
#include <nlohmann/json.hpp>
#include <systems/BrownianPhysics.h>
#include <systems/Gravitation.h>
#include <systems/Physics.h>
#include <systems/UpdateRenderer.h>
#include <components/OrbitalParameters.h>
enum class Scenario
{
    GRAV,
    GRAV_TEST,
    ORBITS,
};

void saveScene(std::string_view file, Scenario scenario)
{
    Scene              scene;
    std::random_device rd{};
    std::mt19937       gen{rd()};


    std::uniform_int_distribution<unsigned> int_dist{0u, 100u};
    std::normal_distribution<double>        normal_dist{10, 1.5};
    std::normal_distribution<double>        normal_dist2{0, 2.5};
    if (scenario == Scenario::GRAV)
    {
        for (int i = 100; i-- > 0;)
        {
            const float radius = normal_dist2(gen) * 100.0;
            scene.makeEntity<StaticTransform, Kinematic, RenderModel>({gmtl::Vec3d{sin(i / 400.0) * radius, cos(i / 400.0) * radius, normal_dist2(gen)}, gmtl::EulerAngleZXYd{}}, {.velocity = {0.45, 0.0, 0.0}, .mass = normal_dist(gen)}, {.path = "sphere", .offset{0, 0, 0}});
        }
    }

    else if (scenario == Scenario::ORBITS)
    {
        scene.makeEntity<OrbitalParameters, RenderModel>({1.0, 5000, 0.0, 0.0, 0.0}, {.path = "orbit"});
    }

    else if (scenario == Scenario::GRAV_TEST)
    {
        scene.makeEntity<StaticTransform, Kinematic, RenderModel>({gmtl::Vec3d{-5, 0, 0}, gmtl::EulerAngleZXYd{}}, {.mass = 5.0}, {.path = "sphere", .offset{0, 0, 0}});
        scene.makeEntity<StaticTransform, Kinematic, RenderModel>({gmtl::Vec3d{5, 0, 0}, gmtl::EulerAngleZXYd{}}, {.mass = 5}, {.path = "sphere", .offset{0, 0, 0}});
    }
    auto s1 = std::ofstream{std::string(file)};
    auto x1 = cereal::JSONOutputArchive(s1);
    scene.save<StaticTransform, Kinematic, RenderModel, OrbitalParameters>(scene.getRegistry(), x1);
}


struct SceneDescriptor
{
    using SystemName = std::string;
    std::string             name;
    std::string             scene_file;
    std::vector<SystemName> systems;


    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SceneDescriptor, name, systems, scene_file)
};

class Simulation
{

public:
    Simulation()
    {
        // for now fixed prototypes, later from plugins
        system_factory.registerPrototype("physics", common::GenericFactory<Physics, Scene&>::proto());
        system_factory.registerPrototype("brownian", common::GenericFactory<BrownianPhysics, Scene&>::proto());
        system_factory.registerPrototype("gravitation", common::GenericFactory<Gravitation, Scene&>::proto());
        system_factory.registerPrototype("update_renderer", common::GenericFactory<UpdateRenderer, Scene&>::proto(updateQueue));
    }

    void initialize(const SceneDescriptor& scene_desc)
    {
        // initialize systems
        for (const auto& system_name : scene_desc.systems)
        {
            systems.emplace_back(system_factory.make(system_name, scene));
        }
        {
            auto stream   = std::ifstream{scene_desc.scene_file};
            auto iarchive = cereal::JSONInputArchive(stream);
            scene.load<StaticTransform, Kinematic, RenderModel, OrbitalParameters>(scene.getRegistry(), iarchive);
        }
        // create rendermodels


        viewer.setup(updateQueue);
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
    using SystemsFactory = common::GenericFactory<System, Scene&>;

    Scene                                scene;
    ViewerCore                           viewer;
    std::vector<std::shared_ptr<System>> systems;
    UpdateQueue                          updateQueue;
    std::uint64_t                        frame_number = 0;
    SystemsFactory                       system_factory;
};

int main(int argc, char** argv)
{
    if (1)
    {

        std::ofstream  test("data/scene1.json");
        nlohmann::json j;
        j["scene"] = nlohmann::json(SceneDescriptor{.name = "sample_scene", .scene_file = "data/ent1.se", .systems{"physics", "update_renderer", "gravitation"}});
        test << j;

        saveScene("data/ent1.se", Scenario::ORBITS);
    }
    std::ifstream storage("data/scene1.json");

    nlohmann::json j = nlohmann::json::parse(storage);


    Simulation sim;
    sim.initialize(j["scene"].get<SceneDescriptor>());
    sim.frame();


    return 0;
}