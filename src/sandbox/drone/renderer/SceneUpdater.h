#pragma once
#include "LoadOperation.h"
#include "SceneObject.h"
#include "components/RenderModel.h"
#include "components/StaticTransform.h"
#include "core/Scene.h"

#include <nlohmann/json.hpp>
#include <thread>
#include <vsg/all.h>

template <class DATA_TYPE>
vsg::t_vec3<DATA_TYPE> gmtlToVsg(const gmtl::Vec<DATA_TYPE, 3>& v)
{
    return vsg::t_vec3(v[0], v[1], v[2]);
}

template <class DATA_TYPE>
vsg::vec3 gmtlToVsgf(const gmtl::Vec<DATA_TYPE, 3>& v)
{
    return vsg::vec3(static_cast<float>(v[0]), static_cast<float>(v[1]), static_cast<float>(v[2]));
}

class SceneUpdater : public vsg::Inherit<vsg::Visitor, SceneUpdater>
{
public:
    SceneUpdater(vsg::ref_ptr<vsg::Group> root, vsg::ref_ptr<vsg::Viewer> viewer, const Scene& scene)
        : root(root)
        , viewer(viewer)
        , threads(vsg::OperationThreads::create(1, viewer->status))
        , scene(scene)
    {
        if (!root)
            throw std::invalid_argument("Root mustn't be a nullptr");
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

    void apply(vsg::FrameEvent& frame)
    {
        auto view = scene.getRegistry().view<RenderModel, StaticTransform>();
        for (auto entity : view)
        {
            auto [transform, model] = view.get<StaticTransform, RenderModel>(entity);

            if (!objects.contains(entity))
            {

                builder->options = {};
                // create
                auto new_object = factory(model);
                root->addChild(new_object);
                objects.emplace(entity, new_object);
            }
            vsg::vec3 position = gmtlToVsgf(transform.position);

            objects.at(entity)->update(position);
        }
        root->accept(*this);
    }
    /**
    void apply_old(vsg::FrameEvent& frame) override
    {


        UpdateQueue<Update>::Queue current;

        queue.swap(current);

        for (const auto& record : current)
        {
            std::string    object_key = (record.key);
            nlohmann::json j          = nlohmann::json::parse(record.value);

            // extract spatial
            auto      x    = j["spatial"]["pos"];
            auto      type = j["type"];
            vsg::vec3 v(x[0], x[1], x[2]);

            // check if we need to create it
            if (!objects.contains(object_key))
            {
                auto new_object  = SceneObject::create();
                auto builder     = vsg::Builder::create();
                builder->options = {};

                if (type == "cone")
                {
                    threads->add(CompileOperation::create(viewer, new_object, builder->createCone()));
                }
                else if (type == "cube")
                {
                    threads->add(CompileOperation::create(viewer, new_object, builder->createBox()));
                }
                else if (type == "sphere")
                {
                    threads->add(CompileOperation::create(viewer, new_object, builder->createSphere()));
                }
                else
                {
                    threads->add(CompileOperation::create(viewer, new_object, builder->createCylinder()));
                }

                auto                     parent = object_key.substr(0, object_key.find_last_of("."));
                vsg::ref_ptr<vsg::Group> attachment_point;
                if (parent == object_key)
                {
                    attachment_point = root;
                }
                else
                {
                    attachment_point = objects.at(parent);
                }
                attachment_point->addChild(new_object);

                objects.insert({object_key, new_object});
            }
            // update
            objects.at(object_key)->update(v);
        }


        if (root)

            root->accept(*this);
    }
    */

private:
    vsg::ref_ptr<vsg::Group>                                    root;
    vsg::observer_ptr<vsg::Viewer>                              viewer;
    vsg::ref_ptr<vsg::OperationThreads>                         threads;
    vsg::ref_ptr<vsg::Builder>                                  builder = vsg::Builder::create();
    const Scene&                                                scene;
    std::unordered_map<entt::entity, vsg::ref_ptr<SceneObject>> objects;
};
