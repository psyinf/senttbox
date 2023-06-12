#pragma once
#include "Helpers.h"
#include "LoadOperation.h"
#include "SceneObject.h"
#include "components/CentralBody.h"
#include "components/OrbitalParameters.h"
#include "components/Orbiter.h"
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
        geom_info.position  = gmtlToVsg(rm.offset);
        geom_info.transform = vsg::scale(rm.scale);

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
        auto new_object = SceneObject::create();
        auto node       = createOrbit(orbit, {});
        threads->add(CompileOperation::create(viewer, new_object, node));
        return new_object;
    }

    void updateOrbit(entt::registry& r, entt::entity e)
    {
        vsg::ref_ptr<vsg::Group> parent;
        auto                     cbr = registry.try_get<CentralBodyRef>(e);

        if (cbr != nullptr && cbr->central_body != entt::null)
        {
            parent = objects[cbr->central_body];
        }
        else
        {
            parent = root;
        }
        // TODO: use parent to remove itself and re-attach
        auto obj  = objects.at(e);
        auto iter = std::find(parent->children.begin(), parent->children.end(), obj);
        if (iter != parent->children.end())
        {
            auto old   = *iter;
            auto orbit = makeOrbit(registry.get<OrbitalParameters>(e));
            objects[e] = orbit;
            *iter      = orbit;
        }
    }
    void removeOrbit(entt::registry& r, entt::entity e)
    {
        auto                     obj = objects.at(e);
        auto                     cbr = registry.try_get<CentralBodyRef>(e);
        vsg::ref_ptr<vsg::Group> parent;
        if (cbr != nullptr && cbr->central_body != entt::null)
        {
            parent = objects[cbr->central_body];
        }
        else
        {
            parent = root;
        }
        parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), obj), parent->children.end());
    }


    void apply(vsg::FrameEvent& frame) override
    {

        // all RenderModels
        for (const auto& [entity, pos, rm] : registry.view<StaticTransform, RenderModel>().each())
        {
            auto cbr = registry.try_get<Orbiter>(entity);
            // root entity but parent not yet registered
            if (cbr != nullptr && cbr->orbit != entt::null && !objects.contains(cbr->orbit) && !objects.contains(entity))
            {
                continue;
            }
            if (!objects.contains(entity))
            {
                builder->options = {};
                // create
                auto new_object = factory(rm);
                // special handling of orbits. TODO: maybe generalize root entities
                if (cbr != nullptr)
                {
                    objects[cbr->orbit]->addChild(new_object);
                }
                else
                {
                    root->addChild(new_object);
                }

                objects.emplace(entity, new_object);
            }

            objects.at(entity)->update(gmtlToVsgd(pos.position));
        }
        // all Orbits

        for (const auto& [entity, orbit] : registry.view<OrbitalParameters>().each())
        {
            auto cbr = registry.try_get<CentralBodyRef>(entity);
            if (cbr != nullptr && cbr->central_body != entt::null && !objects.contains(cbr->central_body) && !objects.contains(entity))
            {
                continue;
            }

            if (!objects.contains(entity))
            {

                registry.on_update<OrbitalParameters>().connect<&DirectUpdater::updateOrbit>(this);
                registry.on_destroy<OrbitalParameters>().connect<&DirectUpdater::removeOrbit>(this);
                // create
                auto new_object = makeOrbit(orbit);
                if (cbr != nullptr)
                {
                    objects[cbr->central_body]->addChild(new_object);
                }
                else
                {
                    root->addChild(new_object);
                }
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
