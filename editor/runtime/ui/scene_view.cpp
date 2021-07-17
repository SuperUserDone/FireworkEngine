#include "scene_view.hpp"

#include <imgui.h>

void scene_view::update(glm::uvec2 &size, size_t id)
{
    ImGui::Begin("Scene");

    ImVec2 wsize{(ImGui::GetWindowSize().x - 15), (ImGui::GetWindowSize().y - 40)};

    size = {wsize.x, wsize.y};

    ImGui::Image((ImTextureID)(uint64_t)id, wsize, ImVec2(0, 1), ImVec2(1, 0));

    ImGui::End();
}

scene_view::~scene_view() {}

scene_view::scene_view() {}
