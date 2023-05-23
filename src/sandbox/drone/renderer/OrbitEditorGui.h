#pragma once

#include <vsgImGui/RenderImGui.h>
#include <vsgImGui/SendEventsToImGui.h>
#include <vsgImGui/Texture.h>
#include <vsgImGui/imgui.h>
#include <vsg/all.h>
#include <fmt/format.h>
#include <components/OrbitalParameters.h>
#include "imgui_entt_entity_editor.h"

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
    //modify a copy to allow for patching, thus triggering an on_update signal
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

} // namespace MM



class OrbitEditorGui : public vsg::Inherit<vsg::Command, OrbitEditorGui>
{
public:
    entt::registry&                 registry;
    mutable MM::EntityEditor<entt::entity> editor;
    mutable entt::entity                    e;
    OrbitEditorGui(entt::registry& reg)
        : registry(reg)
    {
        editor.registerComponent<OrbitalParameters>("Transform");
    }

   
    void record(vsg::CommandBuffer& cb) const override
    {
        ImGui::Begin("Orbit Editor");
		//editor.(registry);
       
        editor.renderSimpleCombo(registry, e);
        ImGui::End();
        

        
    }
};