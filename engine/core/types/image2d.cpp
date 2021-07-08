#include "image2d.hpp"
#include "serialize/compression.hpp"
#include "serialize/image.capnp.h"
#include "serialize/vec_readers.hpp"
#include "vfs/vfs.hpp"

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

namespace fw {

bool image2d::load_from_file(const std::string &vfs_path)
{
    FILE *fp = vfs::vfs_fopen(vfs_path, "r");

    if (!fp) return false;

    ::capnp::PackedFdMessageReader msg(fileno(fp));

    capnp::Image::Reader scene_ser = msg.getRoot<capnp::Image>();

    if (!scene_ser.hasSize()) return false;
    if (!scene_ser.hasCompressedData()) return false;

    auto size = scene_ser.getSize();
    read_vec2(m_size, size);

    auto fmt_capnp = scene_ser.getCompression();
    compression_format fmt;

    switch (fmt_capnp) {
    case capnp::Image::Compression::COMP_NONE:
        fmt = COMPRESSED_NONE;
        break;
    case capnp::Image::Compression::COMP_LZ4:
        fmt = COMPRESSED_LZ4;
        break;
    default:
        FIREWORK_ASSERT(false, "Compression not implemented");
    }

    if (m_data != nullptr) delete m_data;

    m_data = decomp_data(scene_ser.getCompressedData().begin(), fmt);

    auto c_format = scene_ser.getColorFormat();

    switch (c_format) {
    case capnp::Image::ColorFormat::FORMAT_R:
        m_color_format = FORMAT_R;
        break;
    case capnp::Image::ColorFormat::FORMAT_RG:
        m_color_format = FORMAT_RG;
        break;
    case capnp::Image::ColorFormat::FORMAT_RGB:
        m_color_format = FORMAT_RGB;
        break;
    case capnp::Image::ColorFormat::FORMAT_RGBA:
        m_color_format = FORMAT_RGBA;
        break;
    }

    fclose(fp);
    return true;
}

ref<texture2d> &image2d::get_as_texture()
{
    if (m_tex == nullptr) {
        FIREWORK_ASSERT(m_data, "Data cannot be null");
        m_tex = make_ref<texture2d>(m_size.x, m_size.y, m_data, m_color_format);
    }

    return m_tex;
}

image2d::~image2d()
{
    if (m_data != nullptr) {
        delete m_data;
    }
}

} // namespace fw