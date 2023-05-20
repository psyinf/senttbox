#pragma once
#include "DirectUpdater.h"
#include <vsgImGui/RenderImGui.h>
#include <functional>
#include <vsg/all.h>
#include "Gui.h"

class ViewerCore
{


public:
    void setup(entt::registry& reg)
    {
        // options->paths         = {R"(e:\develop\install\vsgRenderSandbox\bin\data\)"};
        options->sharedObjects = vsg::SharedObjects::create();

        auto shaderSet              = vsg::createFlatShadedShaderSet(options);
        auto graphicsPipelineConfig = vsg::GraphicsPipelineConfigurator::create(shaderSet);
        graphicsPipelineConfig->init();

        vsg::ref_ptr<vsg::Window> window(vsg::Window::create(vsg::WindowTraits::create()));
        if (!window)
        {
            throw std::runtime_error("Failed to initialize the window");
        }

        viewer->addWindow(window);
        // set up the camera
        float radius      = 10.0;
        auto  lookAt      = vsg::LookAt::create(vsg::dvec3(0.0, -radius * 3.5, 0.0), vsg::dvec3(), vsg::dvec3(0.0, 0.0, 1.0));
        auto  perspective = vsg::Perspective::create(30.0, static_cast<double>(window->extent2D().width) / static_cast<double>(window->extent2D().height), /*nearfar ratio*/ 0.00001, 10000.0);

        camera = vsg::Camera::create(perspective, lookAt, vsg::ViewportState::create(window->extent2D()));

        updater = DirectUpdater::create(sceneRoot, viewer, reg); // SceneUpdater::create(sceneRoot, viewer, queue);
        viewer->addEventHandler(updater);

        auto commandGraph = vsg::CommandGraph::create(window);
        auto renderGraph  = vsg::RenderGraph::create(window);
        commandGraph->addChild(renderGraph);

        // create the normal 3D view of the scene
        auto view = vsg::View::create(camera);
        view->addChild(vsg::createHeadlight());
        view->addChild(sceneRoot);

        renderGraph->addChild(view);

        {
            auto params      = Params::create();
            auto renderImGui = vsgImGui::RenderImGui::create(window, MyGui::create(params, options));
            renderGraph->addChild(renderImGui);
            // Add the ImGui event handler first to handle events early
            viewer->addEventHandler(vsgImGui::SendEventsToImGui::create());
        }

        viewer->addEventHandler(vsg::CloseHandler::create(viewer));
        viewer->addEventHandler(vsg::Trackball::create(camera));
        viewer->assignRecordAndSubmitTaskAndPresentation({commandGraph});
        viewer->compile();
    }


    bool frame()
    {
        if (firstFrame)
        {
            firstFrame = false;
        }
        bool advance = viewer->advanceToNextFrame();
        if (advance)
        {
            // pass any events into EventHandlers assigned to the Viewer
            viewer->handleEvents();

            viewer->update();

            viewer->recordAndSubmit();

            viewer->present();
        }
        return advance;
    }


private:
    vsg::ref_ptr<vsg::Viewer>  viewer    = vsg::Viewer::create();
    vsg::ref_ptr<vsg::Options> options   = vsg::Options::create();
    vsg::ref_ptr<vsg::Group>   sceneRoot = vsg::Group::create();
    vsg::ref_ptr<vsg::Visitor> updater;

    vsg::ref_ptr<vsg::Camera> camera;
    bool                      firstFrame = true;
};