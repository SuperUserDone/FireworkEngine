#include "core/input.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "material_panel.hpp"
#include "parts/imgui_utils.hpp"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

material_panel::material_panel(fw::scene_manager *man)
    : m_man(man), m_add("Create a new material", "Name", &m_add_name),
      m_error("Error!", "Could not add material name already used!")
{}

void material_panel::update()
{
    if (!m_show) return;

    fw::scene *scene = m_man->get_active_scene();

    ImGui::Begin(u8"Materials\t\uf042", &m_show);

    if (m_editing) {
        if (m_editing_ref == nullptr || m_editing_name == "") {
            m_editing_ref = nullptr;
            m_editing = false;

            m_editing_name = "";
            return;
        }
        draw_edit();
    } else {
        draw_all();
    }

    ImGui::End();

    if (m_add.update()) {
        if (scene->get_materials().count(m_add_name)) m_error.show();
        m_editing_ref = scene->get_materials()
                            .emplace(std::make_pair(m_add_name, fw::make_ref<fw::material>()))
                            .first->second;
        m_editing_name = m_add_name;
        m_editing = true;
        scene->set_dirty();
    }
}

void accept_texture(fw::attribute &attr, const std::string &name)
{
    ImGui::InputText((attr.name + "##vec4f" + name).c_str(), &attr.data_texture_name);
    if (ImGui::BeginDragDropTarget()) {

        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("TEXTURE")) {
            std::string name((const char *)payload->Data);

            attr.data_texture_name = name;
        }
        ImGui::EndDragDropTarget();
    }
    if (ImGui::IsItemHovered() && attr.data.d_texture) {
        ImGui::BeginTooltip();
        uint64_t texture = *(uint32_t *)attr.data.d_texture;
        ImGui::Image((ImTextureID)texture, {128.f, 128.f});
        ImGui::EndTooltip();
    }
}

void material_panel::draw_edit()
{
    ImGui::Text("Editing %s", m_editing_name.c_str());

    fw::scene *scene = m_man->get_active_scene();

    if (fw::input::is_key_down(fw::input::KEY_ESCAPE) &&
        (ImGui::IsWindowHovered() || ImGui::IsWindowFocused()))
        m_editing = false;

    if (m_editing_ref->m_edit_data) {
        if (m_editing_ref->m_edit_data->preview_texture) {
            uint64_t id = *(uint32_t *)m_editing_ref->m_edit_data->preview_texture->get_id();
            ImGui::Image((ImTextureID)id, {256.f, 256.f});
        }

        m_editing_ref->m_edit_data->dirty = true;

        m_editing_ref->m_edit_data->preview_size = 256.f;
    }

    ImGui::SameLine();
    ImGui::BeginChild(("##" + m_editing_name).c_str());
    if (ImGui::InputText(
            ("Shader Ref##" + m_editing_name).c_str(), &m_editing_ref->m_shader_named_ref))
        m_editing_ref->m_attribs.clear();
    ImGui::Separator();

    if (ImGui::BeginTable(("##Table" + m_editing_name).c_str(),
                          std::clamp(ImGui::GetContentRegionAvailWidth() / 300.f, 1.f, 64.f),
                          ImGuiTableFlags_ScrollY)) {
        ImGui::TableNextColumn();
        for (auto &&attr : m_editing_ref->m_attribs) {
            switch (attr.type) {
            case fw::ATTRIB_TYPE_BOOL:
                ImGui::Checkbox(attr.name.c_str(), &attr.data.d_bool);
                break;
            case fw::ATTRIB_TYPE_FLOAT:
                ImGui::DragScalar(attr.name.c_str(), ImGuiDataType_Float, &attr.data.d_float);
                break;
            case fw::ATTRIB_TYPE_DOUBLE:
                ImGui::DragScalar(attr.name.c_str(), ImGuiDataType_Double, &attr.data.d_double);
                break;
            case fw::ATTRIB_TYPE_TEXTURE:
                accept_texture(attr, m_editing_name);
                break;
            case fw::ATTRIB_TYPE_VEC2F:
                ImGui::DragScalarN(
                    attr.name.c_str(), ImGuiDataType_Float, glm::value_ptr(attr.data.d_vec2f), 2);
                break;
            case fw::ATTRIB_TYPE_VEC2I:
                ImGui::DragScalarN(
                    attr.name.c_str(), ImGuiDataType_S32, glm::value_ptr(attr.data.d_vec2i), 2);
                break;
            case fw::ATTRIB_TYPE_VEC2U:
                ImGui::DragScalarN(
                    attr.name.c_str(), ImGuiDataType_U32, glm::value_ptr(attr.data.d_vec2u), 2);
                break;
            case fw::ATTRIB_TYPE_VEC3F:
                draw_value_vector3(attr.name, attr.data.d_vec4f, attr.stepsize, attr.min, attr.max);
                break;
            case fw::ATTRIB_TYPE_VEC3I:
                ImGui::DragScalarN(
                    attr.name.c_str(), ImGuiDataType_S32, glm::value_ptr(attr.data.d_vec3i), 3);
                break;
            case fw::ATTRIB_TYPE_VEC3U:
                ImGui::DragScalarN(
                    attr.name.c_str(), ImGuiDataType_U32, glm::value_ptr(attr.data.d_vec3u), 3);
                break;
            case fw::ATTRIB_TYPE_VEC4F:
                draw_value_vector4(attr.name, attr.data.d_vec4f, attr.stepsize, attr.min, attr.max);
                break;
            case fw::ATTRIB_TYPE_VEC4I:
                ImGui::DragScalarN(
                    attr.name.c_str(), ImGuiDataType_S32, glm::value_ptr(attr.data.d_vec4i), 4);
                break;
            case fw::ATTRIB_TYPE_VEC4U:
                ImGui::DragScalarN(
                    attr.name.c_str(), ImGuiDataType_U32, glm::value_ptr(attr.data.d_vec4u), 4);
                break;
            case fw::ATTRIB_TYPE_INT:
                ImGui::DragScalar(attr.name.c_str(), ImGuiDataType_S64, &attr.data.d_int);
                break;
            case fw::ATTRIB_TYPE_UINT:
                ImGui::DragScalar(attr.name.c_str(), ImGuiDataType_U64, &attr.data.d_int);
                break;
            }
            ImGui::TableNextColumn();
        }
        ImGui::EndTable();
    }
    ImGui::EndChild();
}

void material_panel::draw_all()
{
    static int filtered = 0;
    std::string filter = "";
    ImGui::InputText(u8"\uf0b0\tFilter Materials", &filter);

    fw::scene *scene = m_man->get_active_scene();
    static std::vector<std::string> selections;
    static std::vector<thumbnail> thumbs;

    static int size = 0;

    auto &materials = scene->get_materials();

    ImGui::SameLine();
    bool refresh = ImGui::Button("\uf021\t Refresh previews");

    thumbs.clear();

    for (auto &&mat : materials) {
        auto &&[key, val] = mat;

        if (val->m_edit_data) {
            if (val->m_edit_data->preview_texture)
                thumbs.push_back({*(uint32_t *)val->m_edit_data->preview_texture->get_id(), key});

            if (m_size != val->m_edit_data->preview_size || refresh ||
                size != scene->get_textures().size())
                val->m_edit_data->dirty = true;

            size = scene->get_textures().size();

            val->m_edit_data->preview_size = m_size;
        }
    }

    filtered = draw_grid("Material Table",
                         "MATERIAL",
                         thumbs,
                         selections,
                         m_size,
                         filter,
                         [this, scene](const std::string &name) {
                             this->m_editing_ref = scene->get_materials()[name];
                             this->m_editing_name = name;
                             this->m_editing = true;
                         });

    ImGui::Separator();

    if (ImGui::Button(u8"\uf0fe\tAdd")) m_add.show();
    ImGui::SameLine();
    if (ImGui::Button(u8"\ufae7\tDelete")) {
        for (auto &a : selections) {
            scene->get_materials().erase(a);
            scene->set_dirty();
        }
        selections.clear();
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(256.f);
    ImGui::SliderFloat("##MatThumbSize", &m_size, 64.f, 1024.f, u8"\ufb15\tThumb size: %.0f");
}

material_panel::~material_panel() {}
