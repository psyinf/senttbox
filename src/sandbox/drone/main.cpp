#include "core/Scene.h"
#include "renderer/ViewerCore.h"

#include <FactoryRegistry.h>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>
#include <components/CentralBody.h>
#include <components/OrbitalParameters.h>
#include <components/Orbiter.h>
#include <components/serializers/CentralBody_cereal.h>
#include <components/serializers/Kinematic_cereal.h>
#include <components/serializers/OrbitalParameters_cereal.h>
#include <components/serializers/Orbiter_cereal.h>
#include <components/serializers/RenderModel_cereal.h>
#include <components/serializers/StaticTransform_cereal.h>
#include <nlohmann/json.hpp>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <systems/BrownianPhysics.h>
#include <systems/Gravitation.h>
#include <systems/Orbiters.h>
#include <systems/Physics.h>
#include <systems/UpdateRenderer.h>
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

        
        double planet_offset_scale = 50.0;//1000.0;
        double moon_orbit_scale_offset = 1.0; // 10000.0;
       
        CentralBody sun{"The Sun", 1.989e30};
        CentralBody earth{"The earth", 5.9722e24};
        CentralBody moon{"The Moon", 7.348e22};
        // The sun as central body
        auto sun_body = scene.makeEntity<CentralBody, RenderModel, StaticTransform>(std::move(sun), {.path = "sphere", .offset{0, 0, 0}, .scale{695.508e6}}, {});
        // orbit around the sun
        auto o_sun = scene.makeEntity<OrbitalParameters, CentralBodyRef>({0.0, 149.6e9, 0.0, 0.0, 0.0}, {sun_body});
        // earth orbiting sun
        auto earth_body = scene.makeEntity<CentralBody, Orbiter, StaticTransform, RenderModel>(std::move(earth), {o_sun, 0.0}, {}, {.path = "sphere", .offset{0, 0, 0}, .scale{6356.752e3 * planet_offset_scale}});
       
        // moon orbiting earth
        auto o_earth    = scene.makeEntity<OrbitalParameters, CentralBodyRef>({0.0, 0.3844e9 * moon_orbit_scale_offset , 0.0, 0.0, 0.0}, {earth_body});

        auto moon_body = scene.makeEntity<CentralBody, Orbiter, StaticTransform, RenderModel>(std::move(moon), {o_earth, 0.0}, {}, {.path = "sphere", .offset{0, 0, 0}, .scale{1737.5 * planet_offset_scale * planet_offset_scale}});
      
       
    }

    else if (scenario == Scenario::GRAV_TEST)
    {
        scene.makeEntity<StaticTransform, Kinematic, RenderModel>({gmtl::Vec3d{-5, 0, 0}, gmtl::EulerAngleZXYd{}}, {.mass = 5.0}, {.path = "sphere", .offset{0, 0, 0}});
        scene.makeEntity<StaticTransform, Kinematic, RenderModel>({gmtl::Vec3d{5, 0, 0}, gmtl::EulerAngleZXYd{}}, {.mass = 5}, {.path = "sphere", .offset{0, 0, 0}});
    }
    auto s1 = std::ofstream{std::string(file)};
    auto x1 = cereal::JSONOutputArchive(s1);
    scene.save<StaticTransform, Kinematic, RenderModel, OrbitalParameters, Orbiter, CentralBodyRef, CentralBody>(scene.getRegistry(), x1);
}

struct logging
{
    std::string               file_name{"log.h"};
    spdlog::level::level_enum default_level_console{spdlog::level::info};
    spdlog::level::level_enum default_level_file{spdlog::level::info};
};


void setupLogging(const logging& logging_cfg)
{

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink    = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logging_cfg.file_name);

    console_sink->set_level(logging_cfg.default_level_console);
    file_sink->set_level(logging_cfg.default_level_file);

    std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
    auto                          logger = std::make_shared<spdlog::logger>("default", sinks.begin(), sinks.end());

    spdlog::set_default_logger(logger);
}

struct SceneDescriptor
{
    using SystemName = std::string;
    std::string             name;
    std::string             scene_file;
    std::vector<SystemName> systems;


    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SceneDescriptor, name, systems, scene_file)
};

class ScopedTimer
{
public:
    ScopedTimer(ScopedTimer& other)                  = delete;
    ScopedTimer& operator=(const ScopedTimer& other) = delete;
    ScopedTimer(ScopedTimer&& fp)                    = delete;
    ScopedTimer const& operator=(ScopedTimer&& fp)   = delete;


    explicit ScopedTimer(const std::function<void(std::chrono::nanoseconds)>& cb)
        : callback(cb)
    {
        start_time_point = std::chrono::high_resolution_clock::now();
    }

    ~ScopedTimer()
    {
        callback(std::chrono::high_resolution_clock::now() - start_time_point);
    }

private:
    std::chrono::high_resolution_clock::time_point start_time_point;
    std::function<void(std::chrono::nanoseconds)>  callback;
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
        //system_factory.registerPrototype("update_renderer", common::GenericFactory<UpdateRenderer, Scene&>::proto(updateQueue));
        system_factory.registerPrototype("orbiters", common::GenericFactory<Orbiters, Scene&>::proto());
    }

    void initialize(const SceneDescriptor& scene_desc)
    {
        // initialize systems from loaded scene descriptor
        for (const auto& system_name : scene_desc.systems)
        {
            spdlog::info(fmt::format("adding system {}", system_name));
            systems.emplace_back(system_factory.make(system_name, scene));
        }
        // load scene entities
        {
            auto stream   = std::ifstream{scene_desc.scene_file};
            auto iarchive = cereal::JSONInputArchive(stream);
            scene.load<StaticTransform, Kinematic, RenderModel, OrbitalParameters, Orbiter, CentralBodyRef, CentralBody>(scene.getRegistry(), iarchive);
        }


        viewer.setup(scene.getRegistry());
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
            ++frame_number;
            {
                ScopedTimer sys([&](auto t) { ms_systems += t; });
                runSystems();
            }

            {
                ScopedTimer sys([&](auto t) { ms_rendering += t; });
                advance = viewer.frame();
            }

            if (frame_number % 1000 == 0)
            {
                logTimers(ms_systems, ms_rendering);
            }
        }
    }

    void logTimers(std::chrono::nanoseconds& ms_systems, std::chrono::nanoseconds& ms_rendering)
    {
        std::cout << "sys: " << std::chrono::duration_cast<std::chrono::milliseconds>(ms_systems / 10)
                  << "render:  " << std::chrono::duration_cast<std::chrono::milliseconds>(ms_rendering / 10)
                  << "total: " << std::chrono::duration_cast<std::chrono::milliseconds>((ms_systems + ms_rendering) / 10)
                  << std::endl;
        ms_rendering = std::chrono::nanoseconds{};
        ms_systems   = std::chrono::nanoseconds{};
    }


private:
    using SystemsFactory = common::GenericFactory<System, Scene&>;

    Scene                                scene;
    ViewerCore                           viewer;
    std::vector<std::shared_ptr<System>> systems;
    UpdateQueue                          updateQueue;
    std::uint64_t                        frame_number{0};
    SystemsFactory                       system_factory;
};

int main(int argc, char** argv)
{
    if (1)
    {

        std::ofstream  test("data/scene1.json");
        nlohmann::json j;
        j["scene"] = nlohmann::json(SceneDescriptor{
            .name       = "sample_scene", //
            .scene_file = "data/ent1.se", //
            .systems{
                "physics", "gravitation", "orbiters" //
            }                                                           //
        });
        test << j;

        saveScene("data/ent1.se", Scenario::ORBITS);
    }

    setupLogging(logging{});
    std::ifstream storage("data/scene1.json");

    nlohmann::json j = nlohmann::json::parse(storage);


    Simulation sim;
    sim.initialize(j["scene"].get<SceneDescriptor>());
    sim.frame();


    return 0;
}