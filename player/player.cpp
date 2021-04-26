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

    auto &&scene = smr->get_active_scene();

    auto entity = scene.create_entity("Test");
    auto cam = scene.create_entity("Cam");

    blood::component_camera &camera = cam.add_component<blood::component_camera>();

    blood::component_transform &transform = entity.add_component<blood::component_transform>();
    blood::component_mesh &mesh = entity.add_component<blood::component_mesh>();

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
