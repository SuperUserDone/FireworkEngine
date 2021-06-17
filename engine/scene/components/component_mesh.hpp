#pragma once

#include "render/render_data/buffer.hpp"
#include "render/render_data/vertex_array.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace blood {
struct vertex
{
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec3 tan;
    glm::vec2 uvs;
};
struct component_mesh
{
    component_mesh() { set_layout(); }
    component_mesh(const std::vector<vertex> &vert, const std::vector<uint32_t> &index)
        : verticies(vert), indicies(index), m_buf(vert.size() * sizeof(vertex), vert.data()),
          m_index_buf(index.size() * sizeof(uint32_t), index.data())
    {
        set_layout();
    }

    void set_layout()
    {
        std::vector<vao_element> layout;

        vao_element temp;
        temp.normalized = false;
        temp.offset = offsetof(vertex, pos);
        temp.size = sizeof(vertex);
        temp.stride = sizeof(vertex);
        temp.type = LAYOUT_FLOAT3;
        temp.vao_attrib = 0;

        layout.push_back(temp);

        m_vao.add_buffer(m_buf, layout);
    }

    void update()
    {
        m_buf.set_data(verticies.size() * sizeof(vertex), verticies.data());
        m_index_buf.set_data(indicies.size() * sizeof(uint32_t), indicies.data());
        // set_layout();
    }

    std::vector<vertex> verticies;
    std::vector<uint32_t> indicies;

    buffer m_buf;
    buffer m_index_buf;
    vertex_array m_vao;
};

} // namespace blood