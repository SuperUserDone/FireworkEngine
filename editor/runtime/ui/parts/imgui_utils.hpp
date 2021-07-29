#pragma once

#include "core/logger.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

#include <functional>
#include <stdint.h>
#include <string>
#include <vector>

struct thumbnail
{
    uint32_t tex_id;
    std::string name;
    std::string glyph = "";
};

#include <type_traits>

template <typename T, typename = int>
struct has_id : std::false_type
{};

template <typename T>
struct has_id<T, decltype((void)T::x, 0)> : std::true_type
{};
int draw_grid(const std::string &name,
              const std::string &drag_and_drop_name,
              const std::vector<thumbnail> &thumbs,
              std::vector<std::string> &selections,
              float thumb_size,
              const std::string &filter,
              std::function<void(const std::string &)> ondoubleclick);

template <typename T>
void draw_value_vector4(const std::string &name, T &val, float speed, float min, float max)
{
    // Ugly way to generate compiler error if wrong type, as imgui cant check that
    // TODO Investigate
    if (val.x || val.y || val.z || val.w) {
    }

    ImGui::PushID(name.c_str());
    ImGui::DragFloat4(name.c_str(), glm::value_ptr(val), speed, min, max);

    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        ImGui::OpenPopup(name.c_str());

    if (ImGui::BeginPopup(name.c_str())) {
        ImGui::ColorPicker4(name.c_str(), glm::value_ptr(val));
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

template <typename T>
void draw_value_vector3(const std::string &name, T &val, float speed, float min, float max)
{
    // Ugly way to generate compiler error if wrong type, as imgui cant check that
    // TODO Investigate
    if (val.x || val.y || val.z) {
    }

    ImGui::PushID(name.c_str());
    ImGui::DragFloat3(name.c_str(), glm::value_ptr(val), speed, min, max);

    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        ImGui::OpenPopup(name.c_str());

    if (ImGui::BeginPopup(name.c_str())) {
        ImGui::ColorPicker3(name.c_str(), glm::value_ptr(val));
        ImGui::EndPopup();
    }
    ImGui::PopID();
}