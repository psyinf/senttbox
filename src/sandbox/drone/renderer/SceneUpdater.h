#pragma once
#include "LoadOperation.h"
#include "SceneObject.h"
#include "core/Update.h"

#include <nlohmann/json.hpp>
#include <thread>
#include <vsg/all.h>

template <class DATA_TYPE>
vsg::t_vec3<DATA_TYPE> gmtlToVsg(const gmtl::Vec<DATA_TYPE, 3>& v)
{
    return vsg::t_vec3(v[0], v[1], v[2]);
}

template <class DATA_TYPE>
auto gmtlToVsgd(const gmtl::Vec<DATA_TYPE, 3>& v)
{
    return vsg::dvec3(static_cast<double>(v[0]), static_cast<double>(v[1]), static_cast<double>(v[2]));
}

class SceneUpdater : public vsg::Inherit<vsg::Visitor, SceneUpdater>
{
public:
    SceneUpdater(vsg::ref_ptr<vsg::Group> root, vsg::ref_ptr<vsg::Viewer> viewer, UpdateQueue& queue)
        : root(root)
        , viewer(viewer)
        , threads(vsg::OperationThreads::create(1, viewer->status))
        , updateQueue(queue)

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


        for (const auto& [entity, update] : updateQueue.get())
        {
           
            if (!objects.contains(entity))
            {
                builder->options = {};
                // create
                auto new_object = factory(update.model);
                root->addChild(new_object);
                objects.emplace(entity, new_object);
            }
            if (update.remove)
            {
                root->children.erase(std::remove(root->children.begin(), root->children.end(), objects[entity]), root->children.end());
            }

            vsg::dvec3 position = gmtlToVsgd(update.transform.position);
            
            objects.at(entity)->update(position, vsg::dvec3{update.model.scale, update.model.scale, update.model.scale});
            
        }
        root->accept(*this);
    }
  

private:
    vsg::ref_ptr<vsg::Group>            root;
    vsg::observer_ptr<vsg::Viewer>      viewer;
    vsg::ref_ptr<vsg::OperationThreads> threads;
    vsg::ref_ptr<vsg::Builder>          builder = vsg::Builder::create();

    std::unordered_map<entt::entity, vsg::ref_ptr<SceneObject>> objects;
    UpdateQueue&                                                updateQueue;
};
