#include "core/input.hpp"
#include "core/keycodes.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "scene_view.hpp"

void scene_view::update(glm::uvec2 &size,
                        size_t id,
                        fw::scene *scene,
                        uint32_t curr,
                        const glm::mat4 &view,
                        const glm::mat4 &proj,
                        ImGuizmo::OPERATION &opp)
{
    if (!m_show) return;

    ImGui::Begin("Scene\t\uf11b", &m_show);

    static bool local = false;
    if (ImGui::IsWindowHovered()) {
        if (fw::input::is_key_down(fw::input::KEY_r)) opp = ImGuizmo::ROTATE;
        if (fw::input::is_key_down(fw::input::KEY_g)) opp = ImGuizmo::TRANSLATE;
        if (fw::input::is_key_down(fw::input::KEY_s)) opp = ImGuizmo::SCALE;

        if (fw::input::is_key_down(fw::input::KEY_l)) local = true;
        if (fw::input::is_key_down(fw::input::KEY_k)) local = false;
    }

    ImVec2 wsize{(ImGui::GetWindowSize().x - 15), (ImGui::GetWindowSize().y - 40)};

    size = {wsize.x, wsize.y};

    ImVec2 topleft = ImGui::GetCursorScreenPos();

    ImGui::Image((ImTextureID)(uint64_t)id, wsize, ImVec2(0, 1), ImVec2(1, 0));

    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(topleft.x, topleft.y, wsize.x - 1, wsize.y - 1);
    float viewManipulateRight = ImGui::GetWindowPos().x + wsize.x;
    float viewManipulateTop = ImGui::GetWindowPos().y;

    if (curr != 0) {
        auto &trans = scene->get_registry().get<fw::component_transform>((entt::entity)curr);

        glm::mat4 mat = trans;

        ImGuizmo::Manipulate(glm::value_ptr((view)),
                             glm::value_ptr(proj),
                             opp,
                             local ? ImGuizmo::LOCAL : ImGuizmo::WORLD,
                             glm::value_ptr(mat));

        trans.set_matrix(mat);
    }

    ImGui::End();
}

scene_view::~scene_view() {}

scene_view::scene_view() {}
