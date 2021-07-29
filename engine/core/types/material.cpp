#include "material.hpp"
#include "serialize/material.capnp.h"
#include "serialize/vec_readers.hpp"
#include "vfs/vfs.hpp"

#include <Tracy.hpp>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>

namespace fw {
material::material() {}

bool material::save_to_file()
{
    if (m_path == "") return false;
    if (m_shader_named_ref == "") return false;

    ZoneScopedN("Save material to file");

    FILE *fp = vfs::vfs_fopen(m_path.c_str(), "wb");

    ::capnp::MallocMessageBuilder msg;
    fw::capnp::Material::Builder mat_ser = msg.initRoot<fw::capnp::Material>();

    mat_ser.setShaderRef(m_shader_named_ref);

    auto attrs = mat_ser.initAttributes(m_attribs.size());

    for (int i = 0; i < m_attribs.size(); i++) {
        auto attr = attrs[i];

        auto &tosave = m_attribs[i];
        attr.setBindId(tosave.bind_id);
        auto data = attr.initData();

        attr.setName(tosave.name);

        switch (tosave.type) {
        case ATTRIB_TYPE_BOOL:
            data.setBool(tosave.data.d_bool);
            break;
        case ATTRIB_TYPE_INT:
            data.setInt32(tosave.data.d_int);
            break;
        case ATTRIB_TYPE_UINT:
            data.setUint32(tosave.data.d_uint);
            break;
        case ATTRIB_TYPE_FLOAT:
            data.setFloat(tosave.data.d_float);
            break;
        case ATTRIB_TYPE_DOUBLE:
            data.setDouble(tosave.data.d_double);
            break;
        case ATTRIB_TYPE_TEXTURE:
            data.setTexture(tosave.data_texture_name);
            break;
        case ATTRIB_TYPE_VEC2F:
            write_vec2_direct(tosave.data.d_vec2f, data.initVec2f());
            break;
        case ATTRIB_TYPE_VEC2I:
            write_vec2_direct(tosave.data.d_vec2i, data.initVec2i());
            break;
        case ATTRIB_TYPE_VEC2U:
            write_vec2_direct(tosave.data.d_vec2u, data.initVec2u());
            break;
        case ATTRIB_TYPE_VEC3F:
            write_vec3_direct(tosave.data.d_vec3f, data.initVec3f());
            break;
        case ATTRIB_TYPE_VEC3I:
            write_vec3_direct(tosave.data.d_vec3i, data.initVec3i());
            break;
        case ATTRIB_TYPE_VEC3U:
            write_vec3_direct(tosave.data.d_vec3u, data.initVec3u());
            break;
        case ATTRIB_TYPE_VEC4F:
            write_vec4_direct(tosave.data.d_vec4f, data.initVec4f());
            break;
        case ATTRIB_TYPE_VEC4I:
            write_vec4_direct(tosave.data.d_vec4i, data.initVec4u());
            break;
        case ATTRIB_TYPE_VEC4U:
            write_vec4_direct(tosave.data.d_vec4u, data.initVec4u());
            break;
        }
    }

    ::capnp::writePackedMessageToFd(fileno(fp), msg);
    fclose(fp);

    return true;
}

bool material::load_from_file(const std::string &vfs_path)
{
    ZoneScopedN("Load material from file");

    FILE *fp = vfs::vfs_fopen(vfs_path, "rb");

    if (!fp) return false;

    ::capnp::PackedFdMessageReader msg(fileno(fp));

    capnp::Material::Reader mat_ser = msg.getRoot<capnp::Material>();

    m_shader_named_ref = mat_ser.getShaderRef();

    for (auto attr : mat_ser.getAttributes()) {
        attribute new_attr;

        new_attr.bind_id = attr.getBindId();
        new_attr.name = attr.getName();

        auto data = attr.getData();

        switch (data.which()) {
        case capnp::Attribute::Data::INT32:
            new_attr.type = ATTRIB_TYPE_INT;
            new_attr.data.d_int = data.getInt32();
            break;
        case capnp::Attribute::Data::UINT32:
            new_attr.type = ATTRIB_TYPE_INT;
            new_attr.data.d_uint = data.getUint32();
            break;
        case capnp::Attribute::Data::FLOAT:
            new_attr.type = ATTRIB_TYPE_FLOAT;
            new_attr.data.d_float = data.getFloat();
            break;
        case capnp::Attribute::Data::DOUBLE:
            new_attr.type = ATTRIB_TYPE_DOUBLE;
            new_attr.data.d_double = data.getDouble();
            break;
        case capnp::Attribute::Data::TEXTURE:
            new_attr.type = ATTRIB_TYPE_TEXTURE;
            new_attr.data_texture_name = data.getTexture();
            break;
        case capnp::Attribute::Data::VEC2F:
            new_attr.type = ATTRIB_TYPE_VEC2F;
            read_vec2(new_attr.data.d_vec2f, data.getVec2f());
            break;
        case capnp::Attribute::Data::VEC2I:
            new_attr.type = ATTRIB_TYPE_VEC2I;
            read_vec2(new_attr.data.d_vec2i, data.getVec2i());
            break;
        case capnp::Attribute::Data::VEC2U:
            new_attr.type = ATTRIB_TYPE_VEC2U;
            read_vec2(new_attr.data.d_vec2u, data.getVec2u());
            break;
        case capnp::Attribute::Data::VEC3F:
            new_attr.type = ATTRIB_TYPE_VEC3F;
            read_vec3(new_attr.data.d_vec3f, data.getVec3f());
            break;
        case capnp::Attribute::Data::VEC3I:
            new_attr.type = ATTRIB_TYPE_VEC3I;
            read_vec3(new_attr.data.d_vec3i, data.getVec3i());
            break;
        case capnp::Attribute::Data::VEC3U:
            new_attr.type = ATTRIB_TYPE_VEC3U;
            read_vec3(new_attr.data.d_vec3u, data.getVec3u());
            break;
        case capnp::Attribute::Data::VEC4F:
            new_attr.type = ATTRIB_TYPE_VEC4F;
            read_vec4(new_attr.data.d_vec4f, data.getVec4f());
            break;
        case capnp::Attribute::Data::VEC4I:
            new_attr.type = ATTRIB_TYPE_VEC4I;
            read_vec4(new_attr.data.d_vec4i, data.getVec4i());
            break;
        case capnp::Attribute::Data::VEC4U:
            new_attr.type = ATTRIB_TYPE_VEC4U;
            read_vec4(new_attr.data.d_vec4u, data.getVec4u());
            break;
        case capnp::Attribute::Data::BOOL:
            new_attr.type = ATTRIB_TYPE_BOOL;
            new_attr.data.d_bool = data.getBool();
            break;
        }

        m_attribs.push_back(new_attr);
    }

    m_path = vfs_path;

    fclose(fp);
    return true;
}

material::~material() {}
} // namespace fw