#include <cstdio>
#include <memory>

#include <core/loop.hpp>
#include <scene/component_camera.hpp>
#include <scene/component_mesh.hpp>
#include <scene/component_transform.hpp>

int main(int argc, char const *argv[])
{
    blood::loop loop;

    auto smr = loop.get_scenemanager().lock();

    auto scene = smr->get_active_scene().lock();

    auto entity = scene->create_entity("Test");
    auto cam = scene->create_entity("Cam");

    component_camera &camera = cam.add_component<component_camera>();

    component_transform &transform = entity.add_component<component_transform>();
    component_mesh &mesh = entity.add_component<component_mesh>();

    mesh.verticies = {{{0, 0, 0}}, {{1, 1, 0}}, {{1, 0, 0}}};
    mesh.indicies = {0, 1, 2};

    std::shared_ptr<material> mat = std::make_shared<material>();

    mat->shader.frag_source = "#version 450 core\n"
                              "out vec4 FragColor;\n"
                              "in vec4 vertexColor;\n"
                              "void main() { FragColor = vertexColor; }\n";

    mat->shader.vert_source = "#version 450 core\n"
                              "layout(location = 0) in vec3 aPos;\n"
                              "layout(std140, binding = 0) uniform Matrices{\n"
                              "mat4 projection;\n"
                              "mat4 view;\n"
                              "};\n"
                              "uniform mat4 model;\n"
                              "out vec4 vertexColor;\n"
                              "void main() { gl_Position = projection * view  * vec4(aPos, 1.0); "
                              "vertexColor = vec4(0.5, 0.0, 0.0, 1.0); }\n";

    mesh.m_mat = mat;

    loop.start();
    loop.block();

    return 0;
}
