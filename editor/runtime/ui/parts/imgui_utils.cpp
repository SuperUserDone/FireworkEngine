#include "core/logger.hpp"
#include "imgui_utils.hpp"

#include <algorithm>
#include <math.h>

int draw_grid(const std::string &table_name,
              const std::vector<thumbnail> &thumbs,
              std::vector<std::string> &selections,
              float thumb_size,
              const std::string &filter,
              std::function<void(const std::string &)> ondoubleclick)
{
    int filtered = 0;

    if (ImGui::BeginTable(
            table_name.c_str(),
            std::clamp(floor(ImGui::GetContentRegionMax().x / (thumb_size + 16.f)), 1.f, 64.f),
            ImGuiTableFlags_ScrollY,
            ImVec2{-FLT_MIN, -FLT_MIN - ImGui::GetTextLineHeightWithSpacing() - 8.f})) {

        for (auto &&item : thumbs) {
            auto &name = item.name;
            auto &texid = item.tex_id;
            auto &str = item.glyph;

            std::string name_dup = name;
            std::string filter_dup = filter;

            std::for_each(name_dup.begin(), name_dup.end(), [](char &c) { c = tolower(c); });
            std::for_each(filter_dup.begin(), filter_dup.end(), [](char &c) { c = tolower(c); });

            if ((name_dup.find(filter_dup) == std::string::npos) && filter_dup != "") {
                filtered++;
                continue;
            }

            uint64_t id = texid;

            ImGui::TableNextColumn();

            bool contains_selection =
                std::find(selections.begin(), selections.end(), name) != selections.end();

            auto pos = ImGui::GetCursorPos();

            bool clicked = ImGui::Selectable(
                ("##Selectable" + table_name + name).c_str(),
                contains_selection,
                ImGuiSelectableFlags_AllowItemOverlap | ImGuiSelectableFlags_AllowDoubleClick,
                {thumb_size, thumb_size + ImGui::GetTextLineHeightWithSpacing()});

            if (clicked) {

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

            if (clicked && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                selections.clear();
                selections.push_back(name);
                ondoubleclick(name);
            }

            ImGui::SetCursorPos(pos);

            if (str == "")
                ImGui::Image((ImTextureID)id, {thumb_size, thumb_size});
            else {
                static auto &&font = ImGui::GetIO().Fonts->Fonts[1];

                ImGui::PushFont(font);
                float a = ImGui::CalcTextSize(str.c_str(), str.end().base()).x - 10.f;

                auto pos = ImGui::GetCursorPos();
                ImGui::SetCursorPos({pos.x + (a / 2.f), pos.y});

                ImGui::Text("%s", str.c_str());
                ImGui::PopFont();
            }
            ImGui::Text("%s", name.c_str());
        }

        ImGui::EndTable();
    }

    return filtered;
}
