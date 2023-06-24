#pragma once
#include <vsg/all.h>

/**
 * SceneObject represented as transform in scenegraph
 */
class SceneObject : public vsg::Inherit<vsg::MatrixTransform, SceneObject>
{

public:
    SceneObject() = default;

    void update(vsg::dvec3 pos, vsg::dvec3 scale)
    {
        position = pos;

        this->matrix = vsg::translate(position) * vsg::rotate(vsg::dquat{}) * vsg::scale(scale); // pivot* vsg::translate(- vsg::dvec3{0,0,-0});
    }

    void update(vsg::dvec3 pos)
    {
        position = pos;

        this->matrix = vsg::translate(position);
    }
    const vsg::dvec3& getPosition() const
    {
        return position;
    }

private:
    vsg::dvec3 position;
};
