#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

struct vertex
{
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec3 tan;
    uint8_t color_index;
};

struct material
{
    struct
    {
        char *vert_source = nullptr;
        char *frag_source = nullptr;

        struct
        {
            int shader_program = -1;
        } render_data;
    } shader;
};

struct component_mesh
{
    std::vector<vertex> verticies;
    std::vector<uint32_t> indicies;

    std::shared_ptr<material> m_mat;

    struct
    {
        int vertex_buffer = -1;
        int index_buffer = -1;
        int vao = -1;
    } render_data;
};