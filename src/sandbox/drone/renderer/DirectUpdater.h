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
            // node = createLines({});
            node = builder->createSphere(geom_info);
        }
        else if (rm.path == "orbit")
        {
            node = createLines({});
            // node = builder->createSphere(geom_info);
        }
        else
        {
            // TODO: log warning
            node = builder->createCylinder(geom_info);
        }


        threads->add(CompileOperation::create(viewer, new_object, node));
        return new_object;
    }

    auto makeOrbit(const OrbitalParameters& orbit, const RenderModel& rm)
    {
        auto              new_object = SceneObject::create();
        vsg::GeometryInfo geom_info;
        geom_info.position = gmtlToVsg(rm.offset);
        vsg::ref_ptr<vsg::Node> node;
        // create line-geometry for orbit from OrbitalParameters
        node = createOrbit(orbit, {});
        threads->add(CompileOperation::create(viewer, new_object, node));
        return new_object;
    }


    void apply(vsg::FrameEvent& frame)
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
            objects.at(entity)->update(gmtlToVsgd(pos.position), vsg::dvec3{1.0, 1.0, 1.0});
        }
        // all Orbits
        for (const auto& [entity, orbits, rm] : registry.view<OrbitalParameters, RenderModel>().each())
        {
            if (!objects.contains(entity))
            {
                builder->options = {};
                // create
                auto new_object = makeOrbit(orbits, rm);
                root->addChild(new_object);
                objects.emplace(entity, new_object);
            }
            // objects.at(entity)->update(gmtlToVsgd(pos.position), vsg::dvec3{1.0, 1.0, 1.0});
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
