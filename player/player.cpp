#include "core/logger.hpp"
#include "core/native_script.hpp"
#include "imgui.h"

#include <core/input.hpp>
#include <core/loop.hpp>
#include <cstdio>
#include <memory>
#include <scene/components.hpp>
#include <serialize/scene_serializer.hpp>
#include <vfs/vfs.hpp>

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

    fw::vfs::vfs_register("./", "root");

    auto smr = loop.get_scenemanager().lock();

    fw::scene *new_scene = new fw::scene();

    if (fw::scene_serializer::deserialize(new_scene, "root://boot.fwscn")) {
        new_scene->set_dirty();
        smr->stage_scene(new_scene);
        smr->swap();
    } else {
        return -1;
    }

    loop.tickloop();

    return 0;
}
