
#include <entt/entt.hpp>

#include <sstream>
#include <fmt/color.h>
#include <formatter/GmtlFormatter.h>
#include <math/Spatial.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>

#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>

#include <sstream>
#include <vector>
class Core
{
    struct logging
    {
        std::string               file_name             = "log.h";
        spdlog::level::level_enum default_level_console = spdlog::level::info;
        spdlog::level::level_enum default_level_file    = spdlog::level::info;
    };

public:
    Core()
    {
        setupLogging(logging());

        spdlog::info("Core starting ...");
    }
    ~Core()
    {
        spdlog::info("Core shutting down ...");
    }

private:
    void setupLogging(const logging& logging_cfg) const
    {

        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_sink    = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logging_cfg.file_name);

        console_sink->set_level(logging_cfg.default_level_console);
        file_sink->set_level(logging_cfg.default_level_file);

        std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
        auto                          logger = std::make_shared<spdlog::logger>("default", sinks.begin(), sinks.end());

        spdlog::set_default_logger(logger);
    }
};


class Scene
{
public:
    Scene()
    {
    }

    ~Scene()
    {
    }


    entt::registry& getRegistry()
    {
        return registry;
    }

    const entt::registry& getRegistry() const
    {
        return registry;
    }

private:
    entt::registry registry;
};

struct Entity
{
    std::string type;
    std::string name;
};



struct StaticTransform
{
    common::math::Spatial spatial;
};



//////////////////////////////////////////////////////////////////////////
namespace gmtl
{

template <typename Archive, typename T, size_t SIZE>
void serialize(Archive& archive, gmtl::VecBase<T,SIZE>& vec)
{
    for (auto& x : std::span(vec.getData(), SIZE))
    {
        archive(x);
    }
}
template <typename Archive, typename T>
void serialize(Archive& archive, gmtl::VecBase<T, 3>& vec)
{
    archive(cereal::make_nvp("x", vec.mData[0]));
    archive(cereal::make_nvp("y", vec.mData[1]));
    archive(cereal::make_nvp("z", vec.mData[2]));
}
template <typename Archive, typename T, typename ROT_ORDER>
void serialize(Archive& archive, gmtl::EulerAngle<T,ROT_ORDER>& ori)
{
    archive(cereal::make_nvp("x", ori.getData()[0]));
    archive(cereal::make_nvp("y", ori.getData()[1]));
    archive(cereal::make_nvp("z", ori.getData()[2]));
}

}

template <typename Archive>
void serialize(Archive& archive, StaticTransform& entity)
{
    archive(cereal::make_nvp("spatial", entity.spatial));
}
namespace common::math
{
template <typename Archive>
void serialize(Archive& archive, Spatial& spatial)
{
     archive(cereal::make_nvp("pos",spatial.position),cereal::make_nvp("rot",spatial.orientation));
}

}
template <typename Archive>
void serialize(Archive& archive, Entity& entity)
{
    archive(cereal::make_nvp("type", entity.type), cereal::make_nvp("name", entity.name));
}



int main(int argc, char** argv)
{

    Core  core;
    Scene scene;
    auto& scene_reg = scene.getRegistry();

   
      using namespace entt::literals;
    for (int i = 100; i-- > 0;)
    {
        auto e = scene_reg.create();
        scene_reg.emplace<Entity>(e, fmt::format("type_{}", i));
        scene_reg.emplace<StaticTransform>(e, gmtl::Vec3d(0, i / 10.0, 0), gmtl::EulerAngleZXYd());
    }


    for (auto view = scene_reg.view<Entity, StaticTransform>(); auto entity : view)
    {
        auto [ent, trans] = view.get<Entity, StaticTransform>(entity);
        fmt::print(fmt::emphasis::bold | fg(fmt::color::royal_blue), "{} of type {} at {:.2f}\n", ent.name, ent.type, trans.spatial.position);
    }

    {
        std::ofstream storage_e("data/test.oute");
        std::ofstream storage("data/test.out");
        std::ofstream storage2("data/test.out2");
        std::ofstream storagexxx("data/test.outeb", std::ios::binary);
        // output finishes flushing its contents when it goes out of scope
        cereal::JSONOutputArchive output_e{storage_e};
        cereal::JSONOutputArchive output{storage};
        cereal::JSONOutputArchive output2{storage2};
        cereal::BinaryOutputArchive outputxxx{storagexxx};

        entt::snapshot{scene_reg}.entities(outputxxx).component<Entity>(outputxxx).component<StaticTransform>(outputxxx);
        entt::snapshot{scene_reg}.entities(output_e).component<Entity>(output).component<StaticTransform>(output2);
        
       

    }
    {
        entt::registry reg2;
        std::ifstream  storage_xxx("data/test.outeb", std::ios::binary);
        std::ifstream  storage_e("data/test.oute");
        
        std::ifstream storage("data/test.out");
        std::ifstream  storage2("data/test.out2");
        
        cereal::BinaryInputArchive input_xxx{storage_xxx};
        cereal::JSONInputArchive input_e{storage_e};
        cereal::JSONInputArchive input{storage};
        cereal::JSONInputArchive input2{storage2};
        
            // entt::continuous_loader{reg2}.entities(input_e).component<Entity>(input).component<StaticTransform>(input2);
            // TODO: idea: keep an index of the components starting position inside the archive to allow for random access by seeking the stream appropriately
            entt::continuous_loader{reg2}
                .entities(input_xxx);
        entt::continuous_loader{reg2}.component<Entity>(input_xxx).component<StaticTransform>(input_xxx);
        int i = 0;
        for (auto view = reg2.view<StaticTransform,Entity >(); auto entity : view)
        {
            ++i;
            auto&& [ent, stat] = view.get<Entity, StaticTransform>(entity);
            fmt::print(fmt::emphasis::bold | fg(fmt::color::aquamarine), " at {:.2f}\n", stat.spatial.position);
        }
        std::cout << i;
    }
   

    return 0;
}