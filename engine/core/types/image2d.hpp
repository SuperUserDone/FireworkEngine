#pragma once

#include "core/util.hpp"
#include "render/datatypes.hpp"
#include "render/render_data/texture2d.hpp"

#include <glm/glm.hpp>
#include <memory>

namespace fw {

class image2d
{
public:
    image2d() = default;
    image2d(const std::string &vfs_path) { load_from_file(vfs_path); }

    bool load_from_file(const std::string &vfs_path);

    ref<texture2d> &get_as_texture();

    ~image2d();

private:
    glm::uvec2 m_size = {0, 0};
    ref<texture2d> m_tex = nullptr;

    uint8_t *m_data = nullptr;

    color_format m_color_format = FORMAT_RGB;

private:
    // Delete for now
    image2d(const image2d &x) = delete;
    image2d(image2d &&x) = delete;
    image2d &operator=(const image2d &other) = delete;
    image2d &operator=(image2d &&other) = delete;
};

} // namespace fw