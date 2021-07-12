#include "image2d.hpp"
#include "serialize/image.capnp.h"
#include "serialize/vec_readers.hpp"
#include "vfs/vfs.hpp"

#include <Tracy.hpp>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>

namespace fw {

bool image2d::load_from_file(const std::string &vfs_path)
{
    ZoneScopedN("Load Image from file");

    FILE *fp = vfs::vfs_fopen(vfs_path, "rb");
    if (!fp) return false;
    ::capnp::PackedFdMessageReader msg(fileno(fp));
    capnp::Image::Reader image_ser = msg.getRoot<capnp::Image>();

    if (!image_ser.hasSize()) return false;
    if (!image_ser.hasCompressedData()) return false;

    auto size = image_ser.getSize();
    read_vec2(m_size, size);

    auto fmt_capnp = image_ser.getCompression();
    if (m_data != nullptr) delete[] m_data;

    m_data = new uint8_t[image_ser.getCompressedData().size()];
    memcpy(m_data,
           image_ser.getCompressedData().asBytes().begin(),
           image_ser.getCompressedData().size());

    auto c_format = image_ser.getColorFormat();

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
        delete[] m_data;
    }
}

} // namespace fw