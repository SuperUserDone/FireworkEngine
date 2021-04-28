#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "render/deletion_helpers.hpp"

namespace blood
{

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
        const char *vert_source = nullptr;
        const char *frag_source = nullptr;

        struct
        {
            int shader_program = -1;
        } render_data;
    } shader;

    ~material()
    {
        deletion_command cmd;

        if (shader.render_data.shader_program != -1)
        {

            cmd.type = deletion_command::GPU_SHADER_PROGRAM;
            cmd.id = shader.render_data.shader_program;
            deletion_queue::get_queue().push_queue(cmd);
            shader.render_data.shader_program = -1;
        }
    }
};

struct component_mesh
{
    std::vector<vertex> verticies;
    std::vector<uint32_t> indicies;

    std::shared_ptr<material> m_mat;

    bool changed = false;

    struct
    {
        int vertex_buffer = -1;
        int index_buffer = -1;
        int vao = -1;
    } render_data;

    void queue_delete()
    {
        deletion_command cmd;

        if (render_data.index_buffer != -1)
        {
            cmd.type = deletion_command::GPU_BUFFER;
            cmd.id = render_data.index_buffer;
            deletion_queue::get_queue().push_queue(cmd);
            render_data.index_buffer = -1;
        }
        if (render_data.vertex_buffer != -1)
        {
            cmd.type = deletion_command::GPU_BUFFER;
            cmd.id = render_data.vertex_buffer;
            deletion_queue::get_queue().push_queue(cmd);
            render_data.vertex_buffer = -1;
        }
        if (render_data.vao != -1)
        {
            cmd.type = deletion_command::GPU_VAO;
            cmd.id = render_data.vao;
            deletion_queue::get_queue().push_queue(cmd);
            render_data.vao = -1;
        }
    }

    ~component_mesh() { queue_delete(); }
};

} // namespace blood