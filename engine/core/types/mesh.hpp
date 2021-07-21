#pragma once

#include "render/render_data/buffer.hpp"
#include "render/render_data/vertex_array.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace fw {

struct vertex
{
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec3 tan;
    glm::vec2 uvs;
};

struct mesh
{
    mesh() {}
    mesh(const std::vector<vertex> &vert, const std::vector<uint32_t> &index)
        : verticies(vert), indicies(index), m_buf(vert.size() * sizeof(vertex), vert.data()),
          m_index_buf(index.size() * sizeof(uint32_t), index.data())
    {
        set_layout();
    }
    mesh(const std::string &vfs_path) { load_from_file(vfs_path); }

    bool load_from_file(const std::string &vfs_path);
    bool save_to_file();

    void set_layout();

    void update();

    std::string m_path = "";

    std::vector<vertex> verticies;
    std::vector<uint32_t> indicies;

    buffer m_buf;
    buffer m_index_buf;
    vertex_array m_vao;

    void *editor_data = nullptr;
};
} // namespace fw