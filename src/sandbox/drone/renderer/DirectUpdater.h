#pragma once
#include "Helpers.h"
#include "LoadOperation.h"
#include "SceneObject.h"
#include "components/OrbitalParameters.h"
#include "components/RenderModel.h"
#include "core/Update.h"

class DirectUpdater : public vsg::Inherit<vsg::Visitor, DirectUpdater>
{
public:
    DirectUpdater(vsg::ref_ptr<vsg::Group> root, vsg::ref_ptr<vsg::Viewer> viewer, entt::registry& reg)
        : root(root)
        , viewer(viewer)
        , threads(vsg::OperationThreads::create(1, viewer->status))
        , registry(reg)

    {
        if (!root)
            throw std::invalid_argument("Root mustn't be a nullptr");
        // TODO: on_delete handling
    }

    auto factory(const RenderModel& rm)
    {
        auto                    new_object = SceneObject::create();
        vsg::ref_ptr<vsg::Node> node;
        vsg::GeometryInfo       geom_info;
        geom_info.position = gmtlToVsg(rm.offset);

        if (rm.path == "cone")
        {
            node = builder->createCone(geom_info);
        }
        else if (rm.path == "cube")
        {
            node = builder->createBox(geom_info);
        }
        else if (rm.path == "sphere")
        {
            node = builder->createSphere(geom_info);
        }
        else
        {
            // TODO: log warning
            node = builder->createCylinder(geom_info);
        }


        threads->add(CompileOperation::create(viewer, new_object, node));
        return new_object;
    }

    auto makeOrbit(const OrbitalParameters& orbit)
    {
        auto              new_object = SceneObject::create();
        auto node = createOrbit(orbit, {});
        threads->add(CompileOperation::create(viewer, new_object, node));
        return new_object;
    }

    void updateOrbit(entt::registry& r, entt::entity e) {

        auto obj = objects.at(e);
        auto iter = std::find(root->children.begin(), root->children.end(), obj);
        if (iter != root->children.end())
        {
            auto old = *iter;
            auto orbit = makeOrbit(registry.get<OrbitalParameters>(e));
            objects[e] = orbit;
            *iter      = orbit;
        }

    }
    void removeOrbit(entt::registry& r, entt::entity e)
    {
        auto obj = objects.at(e);
        root->children.erase(std::remove(root->children.begin(), root->children.end(), obj), root->children.end());
        
    }


    void apply(vsg::FrameEvent& frame) override
    {
        // all RenderModels
        for (const auto& [entity, pos, rm] : registry.view<StaticTransform, RenderModel>().each())
        {
            if (!objects.contains(entity))
            {
                builder->options = {};
                // create
                auto new_object = factory(rm);
                root->addChild(new_object);
                objects.emplace(entity, new_object);
            }
            objects.at(entity)->update(gmtlToVsgd(pos.position), vsg::dvec3{rm.scale, rm.scale, rm.scale});
        }
        // all Orbits
        //TODO: update orbits via entt update mechanism
        for (const auto& [entity, orbits] : registry.view<OrbitalParameters>().each())
        {
            if (!objects.contains(entity))
            {
                
                registry.on_update<OrbitalParameters>().connect<&DirectUpdater::updateOrbit>(this);
                registry.on_destroy<OrbitalParameters>().connect<&DirectUpdater::removeOrbit>(this);
                // create
                auto new_object = makeOrbit(orbits);
                root->addChild(new_object);
                objects.emplace(entity, new_object);
            }
        }
        root->accept(*this);
    }


private:
    vsg::ref_ptr<vsg::Group>            root;
    vsg::observer_ptr<vsg::Viewer>      viewer;
    vsg::ref_ptr<vsg::OperationThreads> threads;
    vsg::ref_ptr<vsg::Builder>          builder = vsg::Builder::create();

    std::unordered_map<entt::entity, vsg::ref_ptr<SceneObject>> objects;
    entt::registry&                                             registry;
};
