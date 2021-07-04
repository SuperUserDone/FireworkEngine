#pragma once

#include "framebuffer.hpp"

namespace fw {

class framebuffer_screen
{
public:
    framebuffer_screen(glm::uvec2 size)
        : m_depth_stencil(size.x, size.y), m_color_texture(size.x, size.y, FORMAT_RGBA)
    {
        m_draw_fbo.attach_color({m_color_texture.get_id()});
        m_draw_fbo.attach_depth_stencil_rb(m_depth_stencil);

        m_old_size = size;
    }

    void resize(glm::uvec2 size)
    {
        if (m_old_size != size) {
            m_old_size = size;
            m_depth_stencil.resize(size.x, size.y);
            m_color_texture.set_data(size.x, size.y, nullptr, FORMAT_RGBA);

            m_draw_fbo.attach_color({m_color_texture.get_id()});
            m_draw_fbo.attach_depth_stencil_rb(m_depth_stencil);
        }
    }

    texture2d &get_texture() { return m_color_texture; }
    framebuffer &get_fbo() { return m_draw_fbo; }

    ~framebuffer_screen() {}

private:
    glm::uvec2 m_old_size;

    renderbuffer m_depth_stencil;
    texture2d m_color_texture;
    framebuffer m_draw_fbo;
};

} // namespace fw