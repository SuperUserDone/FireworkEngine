#include <cstdio>
#include <math.h>
#include <memory>

#include "runtime/editor_loop.hpp"
#include "runtime/parse_args.hpp"

#include "vfs/vfs.hpp"

#ifdef main
#undef main
#endif

int main(int argc, const char *argv[])
{
    auto args = parse_args(argc, argv);

    if (args.project_location == "")
    {
        print_usage();
        return 0;
    }

    blood::vfs::vfs_register(args.project_location, "root");

    editor_loop loop;

    auto smr = loop.get_scenemanager().lock();

    auto &&scene = smr->get_active_scene();

    auto entity = scene->create_entity("Test");
    auto cam = scene->create_entity("Cam");

    blood::component_camera &camera = cam.add_component<blood::component_camera>();
    blood::component_transform &camera_transform = cam.add_component<blood::component_transform>();

    camera_transform.pos = {0, 0, 3};

    blood::component_transform &transform = entity.add_component<blood::component_transform>();
    blood::component_mesh &mesh = entity.add_component<blood::component_mesh>();

    mesh.verticies = {{{0, 0, 0}}, {{1, 1, 0}}, {{1, 0, 0}}};
    mesh.indicies = {0, 1, 2};

    transform.pos = {0, 0, 2};

    loop.stop();
    loop.block();

    return 0;
}
