
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
#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>
#include <nlohmann/json.hpp>
#include <sstream>
#include <vector>

#include "archive.h"
#include "GmtlJsonSerializer.h"
#include "SpatialJsonSerializer.h"


struct Entity
{
    std::string type;
    std::string name;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Entity, type, name);
};


struct StaticTransform
{
    common::math::Spatial spatial;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(StaticTransform, spatial);
};



int main(int argc, char** argv)
{
    entt::registry scene_reg;
   
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
    std::string a;
    {
        std::ofstream storage("data/test.out");
        // output finishes flushing its contents when it goes out of scope
        //cereal::JSONOutputArchive output{storage};
        NJSONOutputArchive output;
      

        entt::snapshot{scene_reg}.entities(output).component<StaticTransform, Entity>(output);
       
        output.Close();
        storage << output.AsString();
        a = output.AsString();
    }
    
    {
        entt::registry reg2;
        std::ifstream  storage("data/test.out");
    
        nlohmann::json           j = nlohmann::json::parse(storage);
        
        // output finishes flushing its contents when it goes out of scope
        NJSONInputArchive input(a);

        entt::continuous_loader{reg2}.entities(input).component<StaticTransform, Entity>(input);
        for (auto view = reg2.view<StaticTransform>(); auto entity : view)
        {
            auto ent = view.get<StaticTransform>(entity);
            fmt::print(fmt::emphasis::bold | fg(fmt::color::aquamarine), " at {:.2f}\n", ent.spatial.position);
        }

    }
   

    return 0;
}