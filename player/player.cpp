#include "core/logger.hpp"
#include "core/native_script.hpp"
#include "imgui.h"

#include <core/input.hpp>
#include <core/loop.hpp>
#include <cstdio>
#include <memory>
#include <scene/components.hpp>

class test_script : public blood::native_script
{
public:
    test_script() : blood::native_script() {}
    virtual ~test_script() {}

    virtual void on_start() override
    {
        LOG_D("Start");

        trans = &m_entity.get_component<blood::component_transform>();
    }
    virtual void on_tick_update(double deltatime) override
    {
        if (blood::input::is_key_down(blood::input::KEY_w)) trans->pos.y += 0.001 * deltatime;
        if (blood::input::is_key_down(blood::input::KEY_s)) trans->pos.y -= 0.001 * deltatime;
        if (blood::input::is_key_down(blood::input::KEY_a)) trans->pos.x -= 0.001 * deltatime;
        if (blood::input::is_key_down(blood::input::KEY_d)) trans->pos.x += 0.001 * deltatime;
    }
    virtual void on_render_update(double deltatime) override
    {
        // LOG_DF("Render {}", 1000 / deltatime);
    }
    virtual void on_destroy() override { LOG_D("Destroy"); }

private:
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
    blood::component_transform &camera_transform = cam.add_component<blood::component_transform>();

    camera_transform.pos = {0, 0, 3};

    blood::component_transform &transform = entity.add_component<blood::component_transform>();
    blood::component_mesh &mesh = entity.add_component<blood::component_mesh>();
    blood::component_nativescript &script = entity.add_component<blood::component_nativescript>();

    script.bind<test_script>();

    mesh.verticies = {{{0, 0, 0}}, {{1, 1, 0}}, {{1, 0, 0}}};
    mesh.indicies = {0, 1, 2};
    mesh.update();

    transform.pos = {0, 0, 2};

    loop.tickloop();

    return 0;
}
