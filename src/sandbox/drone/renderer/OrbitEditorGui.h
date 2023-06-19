#pragma once

#include "imgui_entt_entity_editor.h"

#include <components/OrbitalParameters.h>
#include <components/Orbiter.h>
#include <components/SceneProperties.h>
#include <components/SimulationState.h>
#include <fmt/format.h>
#include <vsg/all.h>
#include <vsgImGui/RenderImGui.h>
#include <vsgImGui/SendEventsToImGui.h>
#include <vsgImGui/Texture.h>
#include <vsgImGui/imgui.h>

namespace ImGui
{
bool SilderDouble(const char* label, double* v, double v_min, double v_max, const char* format = nullptr, ImGuiSliderFlags flags = 0)
{
    return ImGui::SliderScalar(label, ImGuiDataType_Double, v, &v_min, &v_max, format, flags);
}

bool SilderDoubleAngular(const char* label, double* v, const char* format = nullptr, ImGuiSliderFlags flags = 0)
{
    static double v_min = 0.0;
    static double v_max = 2 * 3.1415;
    return ImGui::SliderScalar(label, ImGuiDataType_Double, v, &v_min, &v_max, format, flags);
}
} // namespace ImGui
namespace MM
{
template <>
void ComponentEditorWidget<OrbitalParameters>(entt::registry& reg, entt::registry::entity_type e)
{
    auto t = reg.get<OrbitalParameters>(e);
    // modify a copy to allow for patching, thus triggering an on_update signal
    bool change = false;
    change |= ImGui::SilderDouble("Eccentricity", &t.eccentricity, 0.0, 0.99999999999);
    change |= ImGui::SilderDoubleAngular("Inclination", &t.incliniation);
    change |= ImGui::SilderDouble("Semi-major axis", &t.semimajor_axis, 0.0, 100.0);
    change |= ImGui::SilderDoubleAngular("longAN", &t.longAN);
    change |= ImGui::SilderDoubleAngular("longPA", &t.longPA);
    change |= ImGui::SilderDoubleAngular("meanLongitude", &t.meanLongitude);
    change |= ImGui::SilderDoubleAngular("longPA", &t.longPA);

    if (change)
    {
        reg.emplace_or_replace<OrbitalParameters>(e, t);
    }
}
template <>
void ComponentEditorWidget<Orbiter>(entt::registry& reg, entt::registry::entity_type e)
{
    auto current_orbit= reg.get<Orbiter>(e);
    auto referenced_orbit = reg.get<OrbitalParameters>(current_orbit.orbit);

    auto change        = ImGui::SilderDouble("epoch", &current_orbit.epoch, 0.0, 1.0);
    if (ImGui::BeginCombo("combo 1", fmt::format("ID: {}", entt::to_integral(current_orbit.orbit)).c_str()))
    {
        auto                     item_current_idx = 0;
        std::vector<entt::registry::entity_type> items;
        for (const auto& [entity, orbit] : reg.view<OrbitalParameters>().each())
        {
            if (current_orbit.orbit == entity)
            {
                item_current_idx = items.size();
            }
            items.push_back(entity);
           
        }

        for (int n = items.size(); n-- > 0;)
        {
            const bool is_selected = (item_current_idx == n);
            if (ImGui::Selectable(fmt::format("ID: {}", entt::to_integral(items[n])).c_str(), is_selected))
                item_current_idx = n;

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
        
        if (items[item_current_idx] != current_orbit.orbit || change)
        {
            current_orbit.orbit = items[item_current_idx];
            reg.emplace_or_replace<Orbiter>(e, current_orbit);
        }

    }

}
} // namespace MM


class OrbitEditorGui : public vsg::Inherit<vsg::Command, OrbitEditorGui>
{
public:
    entt::registry&                        registry;
    mutable MM::EntityEditor<entt::entity> editor;
    mutable entt::entity                   e;
    OrbitEditorGui(entt::registry& reg)
        : registry(reg)
    {
        editor.registerComponent<OrbitalParameters>("OrbitalParameters");
        editor.registerComponent<Orbiter>("Orbits");
    }


    void record(vsg::CommandBuffer& cb) const override
    {
        ImDrawList* drawListR = ImGui::GetWindowDrawList();
        drawListR->AddRectFilled({0,0}, ImVec2(200, 200), IM_COL32(0, 30, 0, 255));
	
      
        {
            bool             p_open       = true;
            static int       location     = 0;
            ImGuiIO&         io           = ImGui::GetIO();
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
            if (location >= 0)
            {
                const float          PAD       = 10.0f;
                const ImGuiViewport* viewport  = ImGui::GetMainViewport();
                ImVec2               work_pos  = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
                ImVec2               work_size = viewport->WorkSize;
                ImVec2               window_pos, window_pos_pivot;
                window_pos.x       =  (work_pos.x + PAD);
                window_pos.y       =  (work_pos.y + PAD);
                window_pos_pivot.x =  0.0f;
                window_pos_pivot.y =  0.0f;
                ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
                window_flags |= ImGuiWindowFlags_NoMove;
            }
          
            ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
            if (ImGui::Begin("Example: Simple overlay", &p_open, window_flags))
            {
                auto sim_state = registry.ctx().get<SimulationState>();
               // IMGUI_DEMO_MARKER("Examples/Simple Overlay");
                ImGui::Text("Simple overlay\n"
                            "(right-click to change position)");
                ImGui::Separator();
                if (ImGui::IsMousePosValid())
                    ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
                else
                    ImGui::Text("Mouse Position: <invalid>");
                ImGui::Text("Universe Time: %s  (%i)", sim_state.time.format().c_str(), sim_state.time.seconds);
               
            }
            ImGui::End();
        }
        editor.renderSimpleCombo(registry, e);
        ImGui::Begin("SceneProperties");
        auto & scene_properties = registry.ctx().get<SceneProperties>();
        ImGui::SilderDouble("TimeScale", &scene_properties.timestep_scale, 1.0, 100000000.0);
       
        ImGui::End();
    }
};