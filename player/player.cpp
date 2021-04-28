#include "core/logger.hpp"
#include "core/native_script.hpp"
#include "imgui.h"
#include "scene/component_nativescript.hpp"
#include "scene/component_ui_controller.hpp"
#include <cstdio>
#include <memory>

#include <core/loop.hpp>
#include <scene/component_camera.hpp>
#include <scene/component_mesh.hpp>
#include <scene/component_transform.hpp>

class test_script : public blood::native_script
{
public:
    test_script() : blood::native_script() {}
    virtual ~test_script() {}

    void draw_ui()
    {
        ImGui::Begin("Test");

        ImGui::DragFloat3("Pos", &this->trans->pos.x, 0.01);
        ImGui::Text("FrameTime %.3fms (%.1f FPS)", render_time, 1000.f / render_time);
        ImGui::Text("TickTime %.3fms (%.1f TPS)", tick_time, 1000.f / tick_time);

        ImGui::End();
    }

    virtual void on_start() override
    {
        LOG_D("Start");

        trans = &m_entity.get_component<blood::component_transform>();

        auto &ui = m_entity.get_component<blood::component_ui_controller>();

        ui.draw = [this]() { this->draw_ui(); };
    }
    virtual void on_tick_update(double deltatime) override
    {
        tick_time = deltatime;

        if (u_frames++ % 300 == 2)
            LOG_DF("Update {}", 1000 / deltatime);
    }
    virtual void on_render_update(double deltatime) override
    {
        render_time = deltatime;

        if (r_frames++ % 300 == 1)
            LOG_DF("Render {}", 1000 / deltatime);
    }
    virtual void on_destroy() override { LOG_D("Destroy"); }

private:
    uint u_frames = 0;
    uint r_frames = 0;

    double tick_time;
    double render_time;

    blood::component_transform *trans;
};

#ifdef main
#undef main
#endif

int main(int argc, char const *argv[])
{
    blood::loop loop;

    auto smr = loop.get_scenemanager().lock();

    auto &&scene = smr->get_active_scene();

    auto entity = scene->create_entity("Test");
    auto cam = scene->create_entity("Cam");

    blood::component_camera &camera = cam.add_component<blood::component_camera>();

    blood::component_transform &transform = entity.add_component<blood::component_transform>();
    blood::component_mesh &mesh = entity.add_component<blood::component_mesh>();
    blood::component_nativescript &script = entity.add_component<blood::component_nativescript>();
    blood::component_ui_controller &ui = entity.add_component<blood::component_ui_controller>();

    script.bind<test_script>();

    mesh.verticies = {{{0, 0, 0}}, {{1, 1, 0}}, {{1, 0, 0}}};
    mesh.indicies = {0, 1, 2};

    std::shared_ptr<blood::material> mat = std::make_shared<blood::material>();

    mat->shader.frag_source = R"(
#version 450 core
out vec4 FragColor;
in vec4 vertexColor;
void main() { FragColor = vertexColor; }
)";

    mat->shader.vert_source = R"(
#version 450 core
layout(location = 0) in vec3 aPos;
layout(std140, binding = 0) uniform Matrices{
mat4 projection;
mat4 view;
};
layout(location = 0) uniform mat4 model;
out vec4 vertexColor;
void main() { gl_Position = projection * view * model * vec4(aPos, 1.0); vertexColor = vec4(0.5, 0.0, 0.0, 1.0); }
)";

    mesh.m_mat = mat;

    transform.pos = {0, 0, 2};

    loop.start();
    loop.block();

    return 0;
}
