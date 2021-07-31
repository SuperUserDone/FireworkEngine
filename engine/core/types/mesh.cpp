#include "core/util.hpp"
#include "mesh.hpp"
#include "serialize/mesh.capnp.h"
#include "serialize/vec_readers.hpp"
#include "vfs/vfs.hpp"

#include <Tracy.hpp>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>

namespace fw {

bool mesh::save_to_file()
{
    if (m_path == "") return false;

    ZoneScopedN("Save mesh to file");

    FILE *fp = vfs::vfs_fopen(m_path.c_str(), "wb");

    if (!fp) {
        LOG_EF("Could not open VFS file for writing {}", m_path);
        return false;
    }

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

bool mesh::load_from_file(const std::string &vfs_path)
{
    ZoneScopedN("Load mesh from file");

    FILE *fp = vfs::vfs_fopen(vfs_path, "rb");

    if (!fp) {
        LOG_EF("Could not open VFS file for reading {}", vfs_path);
        return false;
    }

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

void mesh::set_layout()
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

void mesh::update()
{
    m_buf.set_data(verticies.size() * sizeof(vertex), verticies.data());
    m_index_buf.set_data(indicies.size() * sizeof(uint32_t), indicies.data());
    set_layout();
}

} // namespace fw