#include "modal.hpp"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

modal::modal(const std::string &title, const std::string &text, std::string *input)
    : m_input(input), m_text(text), m_title(title)
{}

void modal::show() { m_show = true; }

bool modal::update()
{
    bool flag = false;

    if (m_show) ImGui::OpenPopup(m_title.c_str());
    m_show = false;

    if (ImGui::BeginPopupModal(m_title.c_str())) {
        ImGui::Text("%s", m_text.c_str());

        if (m_input) flag = ImGui::InputText("", m_input, ImGuiInputTextFlags_EnterReturnsTrue);
        if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();

        ImGui::SameLine();
        flag |= ImGui::Button("Ok");

        if (flag) ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }

    return flag;
}

modal::~modal() {}
