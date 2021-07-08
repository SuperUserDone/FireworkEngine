#pragma once

#include "core/util.hpp"
#include "render/render_data/buffer.hpp"
#include "render/render_data/vertex_array.hpp"
#include "serialize/mesh.capnp.h"
#include "serialize/vec_readers.hpp"
#include "vfs/vfs.hpp"

#include <capnp/message.h>
#include <capnp/serialize-packed.h>
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

    bool load_from_file(const std::string &vfs_path)
    {
        FILE *fp = vfs::vfs_fopen(vfs_path, "r");

        if (!fp) return false;

        ::capnp::PackedFdMessageReader msg(fileno(fp));

        capnp::Mesh::Reader mesh_ser = msg.getRoot<capnp::Mesh>();

        if (!mesh_ser.hasVerticies()) return false;
        if (!mesh_ser.hasIndicies()) return false;

        auto nverticies = mesh_ser.getVerticies();
        auto nindicies = mesh_ser.getIndicies();

        for (auto vert : nverticies) {
            vertex vert_new;

            read_vertex(vert_new, vert);

            verticies.push_back(vert_new);
        }

        for (auto ind : nindicies) {
            indicies.push_back(ind);
        }

        m_path = vfs_path;

        fclose(fp);
        return true;
    }

    bool save_to_file()
    {
        if (m_path == "") return false;

        FILE *fp = vfs::vfs_fopen(m_path.c_str(), "w");

        ::capnp::MallocMessageBuilder msg;
        fw::capnp::Mesh::Builder mesh_ser = msg.initRoot<fw::capnp::Mesh>();

        auto sverticies = mesh_ser.initVerticies(verticies.size());
        auto sindicies = mesh_ser.initIndicies(indicies.size());

        for (unsigned int i = 0; i < verticies.size(); i++) {
            auto vertex = sverticies[i];
            fw::write_vertex(verticies[i], vertex);
        }

        for (unsigned int i = 0; i < indicies.size(); i++) {
            sindicies.set(i, indicies[i]);
        }

        ::capnp::writePackedMessageToFd(fileno(fp), msg);
        fclose(fp);
        return true;
    }

    void set_layout()
    {
        std::vector<vao_element> layout;

        vao_element temp;
        temp.normalized = false;
        temp.offset = offsetof(vertex, pos);
        temp.stride = sizeof(vertex);
        temp.type = LAYOUT_FLOAT3;
        temp.vao_attrib = 0;
        layout.push_back(temp);

        temp.normalized = true;
        temp.offset = offsetof(vertex, norm);
        temp.vao_attrib = 1;
        layout.push_back(temp);

        temp.offset = offsetof(vertex, tan);
        temp.vao_attrib = 2;
        layout.push_back(temp);

        temp.normalized = false;
        temp.offset = offsetof(vertex, uvs);
        temp.type = LAYOUT_FLOAT2;
        temp.vao_attrib = 3;
        layout.push_back(temp);

        m_vao.add_buffer(m_buf, layout);
    }

    void update()
    {
        m_buf.set_data(verticies.size() * sizeof(vertex), verticies.data());
        m_index_buf.set_data(indicies.size() * sizeof(uint32_t), indicies.data());
        set_layout();
    }

    std::string m_path = "";

    std::vector<vertex> verticies;
    std::vector<uint32_t> indicies;

    buffer m_buf;
    buffer m_index_buf;
    vertex_array m_vao;
};

struct component_mesh
{
    std::string named_ref;
    ref<mesh> mesh_ref;
};

} // namespace fw