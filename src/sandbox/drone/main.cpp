#include "core/Scene.h"
#include "renderer/ViewerCore.h"

#include <systems/BrownianPhysics.h>
#include <systems/Physics.h>

void setupScene(Scene& scene)
{

    scene.makeDrone<StaticTransform, RenderModel>({gmtl::Vec3d{1, 0, 0}, gmtl::EulerAngleZXYd{}}, {"cone"});
    scene.makeDrone<StaticTransform, RenderModel>({gmtl::Vec3d{0, 1, 0}, gmtl::EulerAngleZXYd{}}, {"cone"});
    scene.makeDrone<StaticTransform, RenderModel>({gmtl::Vec3d{0, 0, 1}, gmtl::EulerAngleZXYd{}}, {"cone"});
    scene.makeDrone<StaticTransform, RenderModel>({gmtl::Vec3d{-1, 0, 0}, gmtl::EulerAngleZXYd{}}, {"cone"});
    scene.makeDrone<StaticTransform, RenderModel>({gmtl::Vec3d{0, -1, 0}, gmtl::EulerAngleZXYd{}}, {"cone"});
    scene.makeDrone<StaticTransform, RenderModel>({gmtl::Vec3d{0, 0, -1}, gmtl::EulerAngleZXYd{}}, {"cone"});
    for (int i = 10000; i-- > 0;)
    {
        scene.makeDrone<StaticTransform, Kinematic, RenderModel>({gmtl::Vec3d{0, 0, 0}, gmtl::EulerAngleZXYd{}}, {}, {.path = "sphere", .offset{0, 0, 5}});
    }
}

void runSystems(Scene& s, Physics& p, BrownianPhysics& bp)
{
    p.update(s, 0.016);
    bp.update(s, 0.016);
}

int main(int argc, char** argv)
{
    Scene           scene;
    ViewerCore      core;
    Physics         physics;
    BrownianPhysics brownian;

    core.setup(scene);
    setupScene(scene);


    // TODO: put to thread
    core.run([&]() { runSystems(scene, physics, brownian); }, []() {});


    return 0;
}