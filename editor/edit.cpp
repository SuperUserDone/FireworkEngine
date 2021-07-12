#include "runtime/editor_loop.hpp"
#include "runtime/parse_args.hpp"
#include "vfs/vfs.hpp"

#include <cstdio>
#include <math.h>
#include <memory>

#ifdef main
#undef main
#endif

int main(int argc, const char *argv[])
{
    auto args = parse_args(argc, argv);

    if (args.project_location == "") {
        print_usage();
        return 0;
    }

    fw::vfs::vfs_register(args.project_location, "root");

    editor_loop loop;

    auto smr = loop.get_scenemanager().lock();

    auto &&scene = smr->get_active_scene();

    auto entity = scene->create_entity("Test");
    auto cam = scene->create_entity("Cam");

    fw::component_camera &camera = cam.add_component<fw::component_camera>();
    fw::component_transform &camera_transform = cam.add_component<fw::component_transform>();

    camera_transform.pos = {0, 0, 3};

    fw::component_transform &transform = entity.add_component<fw::component_transform>();
    fw::component_mesh &comp_mesh = entity.add_component<fw::component_mesh>();
    fw::component_material &mat = entity.add_component<fw::component_material>();

    mat.named_ref = "Material1";

    scene->get_meshes()["Mesh1"] = fw::make_ref<fw::mesh>();
    scene->get_materials()["Material1"] = fw::make_ref<fw::material>();

    auto mesh = scene->get_meshes()["Mesh1"];

    mesh->verticies = {{{0, 0, 0}}, {{1, 1, 0}}, {{1, 0, 0}}};
    mesh->verticies[0].uvs = {0, 0};
    mesh->verticies[1].uvs = {1, 1};
    mesh->verticies[2].uvs = {1, 0};

    mesh->indicies = {0, 1, 2};
    mesh->update();

    auto material = scene->get_materials()["Material1"];

    material->m_shader_named_ref = "flat_texmap";

    comp_mesh.named_ref = "Mesh1";

    transform.pos = {0, 0, 2};

    loop.start();
    loop.tickloop();

    return 0;
}
