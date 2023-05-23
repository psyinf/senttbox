#pragma once
#include <vsgImGui/RenderImGui.h>
#include <vsgImGui/SendEventsToImGui.h>
#include <vsgImGui/Texture.h>
#include <vsgImGui/imgui.h>
#include <vsgImGui/implot.h>

#include <vsg/all.h>


struct Params : public vsg::Inherit<vsg::Object, Params>
{
    bool     showGui              = true; // you can toggle this with your own EventHandler and key
    bool     showDemoWindow       = true;
    bool     showSecondWindow     = true;
    bool     showImPlotDemoWindow = true;
    bool     showLogoWindow       = true;
    bool     showImagesWindow     = true;
    float    clearColor[3]{0.2f, 0.2f, 0.4f}; // Unfortunately, this doesn't change dynamically in vsg
    uint32_t counter = 0;
    float    dist    = 0.f;
};

class MyGui : public vsg::Inherit<vsg::Command, MyGui>
{
public:
    vsg::ref_ptr<vsgImGui::Texture> texture;
    vsg::ref_ptr<Params> params;

    MyGui(vsg::ref_ptr<Params> in_params, vsg::ref_ptr<vsg::Options> options = {}) :
        params(in_params)
    {
        auto texData = vsg::read_cast<vsg::Data>("textures/VSGlogo.png", options);
        texture = vsgImGui::Texture::create_if(texData, texData);
    }

    // we need to compile textures before we can use them for rendering
    void compile(vsg::Context& context) override
    {
        if (texture) texture->compile(context);
    }

    // Example here taken from the Dear imgui comments (mostly)
    void record(vsg::CommandBuffer& cb) const override
    {

        
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        if (params->showGui)
        {
            ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

            ImGui::Text("Some useful message here.");                 // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &params->showDemoWindow); // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &params->showSecondWindow);
            ImGui::Checkbox("ImPlot Demo Window", &params->showImPlotDemoWindow);
            if (texture)
            {
                ImGui::Checkbox("Images Window", &params->showImagesWindow);
            }

            ImGui::SliderFloat("float", &params->dist, 0.0f, 1.0f);        // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&params->clearColor); // Edit 3 floats representing a color

            if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
                params->counter++;

            ImGui::SameLine();
            ImGui::Text("counter = %d", params->counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (params->showSecondWindow)
        {
            ImGui::Begin("Another Window", &params->showSecondWindow); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                params->showSecondWindow = false;
            ImGui::End();
        }

        if (params->showDemoWindow)
        {
            ImGui::ShowDemoWindow(&params->showDemoWindow);
        }

        if (params->showImPlotDemoWindow)
        {
            ImPlot::ShowDemoWindow(&params->showImPlotDemoWindow);
        }

        // UV for a squre in the logo texture
        if (texture)
        {
            ImVec2 squareUV(static_cast<float>(texture->height) / texture->width, 1.0f);

            if (params->showLogoWindow)
            {
                // Copied from imgui_demo.cpp simple overlay
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
                const float PAD = 10.0f;
                const ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
                ImVec2 work_size = viewport->WorkSize;
                ImVec2 window_pos, window_pos_pivot;
                window_pos.x = work_pos.x + PAD;
                window_pos.y = work_pos.y + work_size.y - PAD;
                window_pos_pivot.x =  0.0f;
                window_pos_pivot.y =  1.0f;
                ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
                window_flags |= ImGuiWindowFlags_NoMove;
                ImGui::SetNextWindowBgAlpha(0.0f); // Transparent background
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                ImGui::Begin("vsgCS UI", nullptr, window_flags);

                // Display a square from the VSG logo
                const float size = 128.0f;
                ImGui::Image(texture->id(cb.deviceID), ImVec2(size, size), ImVec2(0.0f, 0.0f), squareUV);

                ImGui::End();
                ImGui::PopStyleVar();
            }

            if (params->showImagesWindow)
            {
                ImGui::Begin("Image Window", &params->showImagesWindow);
                ImGui::Text("An texture:");
                // The logo texture is big, show show it at half size

                ImGui::Image(texture->id(cb.deviceID), ImVec2(texture->width / 2.0f, texture->height / 2.0f));

                // We could make another component class for ImageButton, but we will take a short cut
                // and reuse the descriptor set from our existing texture.
                //
                // Make a small square button
                if (ImGui::ImageButton("Button", texture->id(cb.deviceID),
                                    ImVec2(32.0f, 32.0f),
                                    ImVec2(0.0f, 0.0f),
                                    squareUV))
                    params->counter++;

                ImGui::SameLine();
                ImGui::Text("counter = %d", params->counter);
                ImGui::End();
            }
        }
    }
};