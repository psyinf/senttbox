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
bool SilderDouble(const char* label, double* v, double v_min, double v_max, const char* format, ImGuiSliderFlags flags = 0)
{
    return ImGui::SliderScalar(label, ImGuiDataType_Double, v, &v_min, &v_max, format, flags);
}

bool SilderDoubleAngular(const char* label, double* v, const char* format, ImGuiSliderFlags flags = 0)
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
    auto& t = reg.get<OrbitalParameters>(e);
    // the "##Transform" ensures that you can use the name "x" in multiple labels
    ImGui::SilderDouble("Eccentricity", &t.eccentricity, 0.0, 0.99999999999, nullptr);
    ImGui::SilderDoubleAngular("Inclination", &t.incliniation, nullptr);
   
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