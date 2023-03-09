#pragma once
	
/**
In case we fail using cereal:
register typename <-> type relationship in the registry

*/
class Builder
{
public:
    Builder(Scene& s)
        : scene(s)
    {
    }

    virtual void build(nlohmann::json&, entt::entity) = 0;

    Scene& scene;
};


template <class T >
class GenericBuilder : public Builder
{
    void build(nlohmann::json& j, entt::entity e) override
    {
        T t = j["object"];
        scene.getRegistry().emplace_or_replace<T>(e, t);
    }
};



class BuilderRegistry
{
public:
    BuilderRegistry(Scene& s)
        : scene(s)
    {
    }
    void build(const nlohmann::json& j) {
        auto type = j["_type"];   
    }

  

private:
    Scene& scene;
    std::unordered_map<std::string, std::shared_ptr<Builder>> registeredBuilders;
};