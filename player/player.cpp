#include "core/logger.hpp"
#include "core/native_script.hpp"
#include "imgui.h"

#include <core/input.hpp>
#include <core/loop.hpp>
#include <cstdio>
#include <memory>
#include <scene/components.hpp>

class test_script : public fw::native_script
{
public:
    test_script() : fw::native_script() {}
    virtual ~test_script() {}

    virtual void on_start() override
    {
        LOG_D("Start");

        trans = &m_entity.get_component<fw::component_transform>();
    }
    virtual void on_tick_update(double deltatime) override
    {
        if (fw::input::is_key_down(fw::input::KEY_w)) trans->pos.y += 0.001 * deltatime;
        if (fw::input::is_key_down(fw::input::KEY_s)) trans->pos.y -= 0.001 * deltatime;
        if (fw::input::is_key_down(fw::input::KEY_a)) trans->pos.x -= 0.001 * deltatime;
        if (fw::input::is_key_down(fw::input::KEY_d)) trans->pos.x += 0.001 * deltatime;
    }
    virtual void on_render_update(double deltatime) override
    {
        // LOG_DF("Render {}", 1000 / deltatime);
    }
    virtual void on_destroy() override { LOG_D("Destroy"); }

private:
    fw::component_transform *trans;
};

#ifdef main
#undef main
#endif

int main(int argc, char const *argv[])
{
    fw::loop loop;

    auto smr = loop.get_scenemanager().lock();

    auto &&scene = smr->get_active_scene();

    auto entity = scene->create_entity("Test");
    auto cam = scene->create_entity("Cam");

    fw::component_camera &camera = cam.add_component<fw::component_camera>();
    fw::component_transform &camera_transform = cam.add_component<fw::component_transform>();

    camera_transform.pos = {0, 0, 3};

    fw::component_transform &transform = entity.add_component<fw::component_transform>();
    fw::component_mesh &comp_mesh = entity.add_component<fw::component_mesh>();
    fw::component_nativescript &script = entity.add_component<fw::component_nativescript>();

    script.bind<test_script>();

    scene->get_meshes()["Mesh1"] = fw::make_ref<fw::mesh>();

    auto mesh = scene->get_meshes()["Mesh1"];

    mesh->verticies = {{{0, 0, 0}}, {{1, 1, 0}}, {{1, 0, 0}}};
    mesh->indicies = {0, 1, 2};
    mesh->update();

    comp_mesh.mesh_ref = mesh;
    comp_mesh.named_ref = "Mesh1";

    transform.pos = {0, 0, 2};

    loop.tickloop();

    return 0;
}
