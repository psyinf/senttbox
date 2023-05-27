#pragma once

#include "imgui_entt_entity_editor.h"

#include <components/OrbitalParameters.h>
#include <components/Orbiter.h>
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

        ImGui::Begin("Orbit Editor");
        // editor.(registry);

        editor.renderSimpleCombo(registry, e);
        ImGui::End();
    }
};