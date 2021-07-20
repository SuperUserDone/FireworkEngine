#include "imgui_utils.hpp"

#include <algorithm>
#include <math.h>

void draw_grid(const std::vector<std::pair<uint32_t, std::string>> &thumbs,
               std::vector<std::string> &selections,
               float thumb_size)
{
    if (ImGui::BeginTable(
            "Texture Table",
            floor(ImGui::GetContentRegionMax().x / (thumb_size + 16.f)),
            ImGuiTableFlags_ScrollY,
            ImVec2{-FLT_MIN, -FLT_MIN - ImGui::GetTextLineHeightWithSpacing() - 8.f})) {

        for (auto &&item : thumbs) {
            auto &&[texid, name] = item;

            uint64_t id = texid;

            ImGui::TableNextColumn();

            bool contains_selection = std::count(selections.begin(), selections.end(), name);

            auto pos = ImGui::GetCursorPos();

            if (ImGui::Selectable(
                    ("##Selectable" + name).c_str(),
                    &contains_selection,
                    ImGuiSelectableFlags_AllowItemOverlap,
                    {thumb_size, thumb_size + ImGui::GetTextLineHeightWithSpacing()})) {

                if (ImGui::GetIO().KeyCtrl) {
                    if (contains_selection) {
                        auto location = std::find(selections.begin(), selections.end(), name);
                        if (location != selections.end()) selections.erase(location);
                    } else
                        selections.push_back(name);
                } else {
                    selections.clear();
                    selections.push_back(name);
                }
            }

            ImGui::SetCursorPos(pos);

            ImGui::Image((ImTextureID)id, {thumb_size, thumb_size});
            ImGui::Text("%s", name.c_str());
        }

        ImGui::EndTable();
    }
}
