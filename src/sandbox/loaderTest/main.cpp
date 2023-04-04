
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>

#include <entt/entt.hpp>
#include <sstream>


struct Entity
{
    std::string name;
    int         counter;
};

template <typename Archive>
void serialize(Archive& archive, Entity& entity)
{
    archive(cereal::make_nvp("counter", entity.counter), cereal::make_nvp("name", entity.name));
}


int main(int argc, char** argv)
{

    entt::registry reg;

    for (int i = 10; i-->0;)
    {
        auto e = reg.create();
        reg.emplace<Entity>(e, Entity{.name="subject" + std::to_string(i), .counter=1});
    }
 
    std::stringstream storage;
    {
        // initial snapshot
 
        cereal::JSONOutputArchive oa{storage};
        entt::snapshot{reg}.entities(oa).component<Entity>(oa);
 
    }

    

    //perform some updates
    for (auto&& view = reg.view<Entity>(); auto& entity : view)
    {
        auto& ent = view.get<Entity>(entity);
        ent.counter++;
    }

    //restore previous state
    cereal::JSONInputArchive ia{storage};
    entt::continuous_loader{reg}.entities(ia).component<Entity>(ia);

    for (auto&& view = reg.view<Entity>(); const auto& entity : view)
    {
        auto& ent = view.get<Entity>(entity);
        std::cout << ent.counter << std::endl;
    }



        
    return 0;
}