// for the license, see the end of the file
#pragma once

#include <entt/entt.hpp>
#include <functional>
#include <vsgImGui/imgui.h>
#include <map>
#include <set>
#include <string>
#include <fmt/format.h>


constexpr auto MM_IEEE_IMGUI_PAYLOAD_TYPE_ENTITY = "MM_IEEE_ENTITY";


namespace MM
{
//TODO: put to registry
struct EditorState
{
    entt::entity selected{entt::null}; /// the entity currently being edited
};

static EditorState state;
template <class EntityType>
inline void EntityWidget(EntityType& e, entt::basic_registry<EntityType>& reg, bool dropTarget = false)
{
    ImGui::PushID(static_cast<int>(entt::to_integral(e)));

    if (reg.valid(e))
    {
        if (ImGui::Button(fmt::format("ID: {}", entt::to_integral(e)).c_str()))
        {
            state.selected = e;
        }
    }
    else
    {
        ImGui::Text("Invalid Entity");
    }
    
    ImGui::PopID();
}

template <class Component, class EntityType>
void ComponentEditorWidget([[maybe_unused]] entt::basic_registry<EntityType>& registry, [[maybe_unused]] EntityType entity)
{
}

template <class Component, class EntityType>
void ComponentAddAction(entt::basic_registry<EntityType>& registry, EntityType entity)
{
    registry.template emplace<Component>(entity);
}

template <class Component, class EntityType>
void ComponentRemoveAction(entt::basic_registry<EntityType>& registry, EntityType entity)
{
    registry.template remove<Component>(entity);
}

template <class EntityType>
class EntityEditor
{
public:
    using Registry        = entt::basic_registry<EntityType>;
    using ComponentTypeID = entt::id_type;

    struct ComponentInfo
    {
        using Callback = std::function<void(Registry&, EntityType)>;
        std::string name;
        Callback    widget;
        Callback    create;
        Callback    destroy;
    };

    bool show_window = true;

private:
    std::map<ComponentTypeID, ComponentInfo> component_infos;

    bool entityHasComponent(Registry& registry, EntityType& entity, ComponentTypeID type_id)
    {
        const auto* storage_ptr = registry.storage(type_id);
        return storage_ptr != nullptr && storage_ptr->contains(entity);
    }

public:
    template <class Component>
    ComponentInfo& registerComponent(const ComponentInfo& component_info)
    {
        auto index       = entt::type_hash<Component>::value();
        auto insert_info = component_infos.insert_or_assign(index, component_info);
        return std::get<ComponentInfo>(*insert_info.first);
    }

    template <class Component>
    ComponentInfo& registerComponent(const std::string& name, typename ComponentInfo::Callback widget)
    {
        return registerComponent<Component>(ComponentInfo{
            name,
            widget,
            ComponentAddAction<Component, EntityType>,
            ComponentRemoveAction<Component, EntityType>,
        });
    }

    template <class Component>
    ComponentInfo& registerComponent(const std::string& name)
    {
        return registerComponent<Component>(name, ComponentEditorWidget<Component, EntityType>);
    }

    void renderEditor(Registry& registry, EntityType& e)
    {
        ImGui::TextUnformatted("Editing:");
        ImGui::SameLine();

        if (state.selected != entt::null)
        {
            e = state.selected;
        }

        if (ImGui::Button("New"))
        {
            e = registry.create();
        }
        if (registry.valid(e))
        {
            ImGui::SameLine();

            if (ImGui::Button("Clone"))
            {
                auto old_e = e;
                e          = registry.create();

                // create a copy of an entity component by component
                for (auto&& curr : registry.storage())
                {
                    if (auto& storage = curr.second; storage.contains(old_e))
                    {
                        // TODO: do something with the return value. returns false on failure.
                        storage.emplace(e, storage.get(old_e));
                    }
                }
            }
            ImGui::SameLine();

            ImGui::Dummy({10, 0}); // space destroy a bit, to not accidentally click it
            ImGui::SameLine();

            // red button
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.65f, 0.15f, 0.15f, 1.f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.3f, 0.3f, 1.f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.f, 0.2f, 0.2f, 1.f));
            if (ImGui::Button("Destroy"))
            {
                registry.destroy(e);
                e = entt::null;
            }
            ImGui::PopStyleColor(3);
        }

        ImGui::Separator();

        if (registry.valid(e))
        {
            ImGui::PushID(static_cast<int>(entt::to_integral(e)));
            std::map<ComponentTypeID, ComponentInfo> has_not;
            for (auto& [component_type_id, ci] : component_infos)
            {
                if (entityHasComponent(registry, e, component_type_id))
                {
                    ImGui::PushID(component_type_id);
                    if (ImGui::Button("-"))
                    {
                        ci.destroy(registry, e);
                        ImGui::PopID();
                        continue; // early out to prevent access to deleted data
                    }
                    else
                    {
                        ImGui::SameLine();
                    }
                    bool collapsed = (e != entt::null);
                    if (ImGui::CollapsingHeader(ci.name.c_str()), &collapsed)
                    {
                        ImGui::Indent(30.f);
                        ImGui::PushID("Widget");
                        ci.widget(registry, e);
                        ImGui::PopID();
                        ImGui::Unindent(30.f);
                    }
                    ImGui::PopID();
                }
                else
                {
                    has_not[component_type_id] = ci;
                }
            }

            if (!has_not.empty())
            {
                if (ImGui::Button("+ Add Component"))
                {
                    ImGui::OpenPopup("Add Component");
                }

                if (ImGui::BeginPopup("Add Component"))
                {
                    ImGui::TextUnformatted("Available:");
                    ImGui::Separator();

                    for (auto& [component_type_id, ci] : has_not)
                    {
                        ImGui::PushID(component_type_id);
                        if (ImGui::Selectable(ci.name.c_str()))
                        {
                            ci.create(registry, e);
                        }
                        ImGui::PopID();
                    }
                    ImGui::EndPopup();
                }
            }
            ImGui::PopID();
        }
    }

    void renderEntityList(Registry& registry, std::set<ComponentTypeID>& comp_list)
    {
        ImGui::Text("Components Filter:");
        ImGui::SameLine();
        if (ImGui::SmallButton("clear"))
        {
            comp_list.clear();
        }

        ImGui::Indent();

        for (const auto& [component_type_id, ci] : component_infos)
        {
            bool is_in_list = comp_list.count(component_type_id);
            bool active     = is_in_list;

            ImGui::Checkbox(ci.name.c_str(), &active);

            if (is_in_list && !active)
            { // remove
                comp_list.erase(component_type_id);
            }
            else if (!is_in_list && active)
            { // add
                comp_list.emplace(component_type_id);
            }
        }

        ImGui::Unindent();
        ImGui::Separator();

        if (comp_list.empty())
        {
            ImGui::Text("Orphans:");
            registry.each([&registry](auto e) {
                if (registry.orphan(e))
                {
                    EntityWidget(e, registry, false);
                }
            });
        }
        else
        {
            entt::runtime_view view{};
            for (const auto type : comp_list)
            {
                auto* storage_ptr = registry.storage(type);
                if (storage_ptr != nullptr)
                {
                    view.iterate(*storage_ptr);
                }
            }

            // TODO: add support for exclude

            ImGui::Text("%lu Entities Matching:", view.size_hint());

            if (ImGui::BeginChild("entity list"))
            {
                for (auto e : view)
                {
                    EntityWidget(e, registry, false);
                }
            }
            ImGui::EndChild();
        }
    }

    [[deprecated("Use renderEditor() instead. And manage the window yourself.")]] void render(Registry& registry, EntityType& e)
    {
        if (show_window)
        {
            if (ImGui::Begin("Entity Editor", &show_window))
            {
                renderEditor(registry, e);
            }
            ImGui::End();
        }
    }

    // displays both, editor and list
    // uses static internally, use only as a quick way to get going!
    void renderSimpleCombo(Registry& registry, EntityType& e)
    {
        if (show_window)
        {
            ImGui::SetNextWindowSize(ImVec2(550, 400), ImGuiCond_FirstUseEver);
            if (ImGui::Begin("Entity Editor", &show_window))
            {
                if (ImGui::BeginChild("list", {200, 0}, true))
                {
                    static std::set<ComponentTypeID> comp_list;
                    renderEntityList(registry, comp_list);
                }
                ImGui::EndChild();

                ImGui::SameLine();

                if (ImGui::BeginChild("editor"))
                {
                    renderEditor(registry, e);
                }
                ImGui::EndChild();
            }
            ImGui::End();
        }
    }
};

} // namespace MM

// MIT License

// Copyright (c) 2019-2022 Erik Scholz
// Copyright (c) 2020 Gnik Droy

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.