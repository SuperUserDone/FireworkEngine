#include "core/input.hpp"
#include "core/keycodes.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "scene_view.hpp"

void scene_view::update(glm::uvec2 &size,
                        size_t id,
                        fw::scene *scene,
                        uint32_t curr,
                        ImGuizmo::OPERATION &opp,
                        editor_camera &cam)
{
    if (!m_show) return;

    const glm::mat4 view = cam;
    const glm::mat4 proj = cam.cam.get_projection(size.x, size.y);

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
    float viewManipulateTop = ImGui::GetWindowPos().y;
    float viewManipulateRight = ImGui::GetWindowPos().x + wsize.x;
    camera(cam, viewManipulateRight, viewManipulateTop);

    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(topleft.x, topleft.y, wsize.x - 1, wsize.y - 1);

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

void scene_view::camera(editor_camera &cam, float x, float y)
{
    glm::mat4 view = (glm::mat4)cam;

    ImGuizmo::ViewManipulate(glm::value_ptr(view),
                             glm::distance(cam.pos, cam.target),
                             {x - 128, y + 32},
                             {128, 128},
                             0x0);

    glm::vec3 a;
    glm::vec4 b;
    glm::quat c;

    cam.target = {0, 0, 0};

    glm::decompose(glm::inverse(view), a, c, cam.pos, a, b);

    if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
        auto delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);

        glm::mat4 rx, ry, roll;

        rx = glm::rotate(glm::mat4(1.f), -delta.x * 0.01f, glm::vec3(0.f, 1.f, 0.f));
        ry = glm::rotate(glm::mat4(1.f), -delta.y * 0.01f, glm::vec3(glm::inverse(view)[0]));

        roll = rx * ry;

        glm::vec4 new_dir = glm::inverse(view)[2];
        new_dir = roll * new_dir;
        new_dir = glm::vec4(glm::normalize(glm::vec3(new_dir)), 0.f);

        glm::vec4 planar_dir =
            glm::vec4(glm::cross(glm::vec3(glm::inverse(view)[0]), glm::vec3(0.f, 1.f, 0.f)), 0.f);
        planar_dir.y = 0.f;
        planar_dir = glm::vec4(glm::normalize(glm::vec3(planar_dir)), 0.f);

        float dt = glm::dot(glm::vec3(planar_dir), glm::vec3(new_dir));

        if (dt < 0.0f) {
            new_dir += planar_dir * dt;
            new_dir = glm::vec4(glm::normalize(glm::vec3(new_dir)), 0.f);
        }

        cam.pos = cam.target + glm::vec3(new_dir * glm::distance(cam.pos, cam.target));

        LOG_DF("{}, {}", cam.pos, dt);
        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Middle);
    }
}

scene_view::~scene_view() {}

scene_view::scene_view() {}
