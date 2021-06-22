#pragma once

#include "render/render_api/render_api.hpp"
#include "renderbuffer.hpp"
#include "texture2d.hpp"

#include <cstring>

namespace blood {

static inline int DEFAULT_FRAMEBUFFER = 0;

class framebuffer
{
public:
    framebuffer() { m_id = render_api_impl::get_api()->alloc_framebuffer(); }

    void attach_color(const std::vector<texture_id> &texture)
    {
        render_api_impl::get_api()->set_fbo_color(m_id, texture);
    }
    void attach_depth_stencil_tex(const texture2d &texture)
    {
        render_api_impl::get_api()->set_fbo_depth_stencil_texture(m_id, texture.get_id());
    }
    void attach_depth_stencil_rb(const renderbuffer &rb)
    {
        render_api_impl::get_api()->set_fbo_depth_stencil_renderbuffer(m_id, rb.get_id());
    }

    void use() { render_api_impl::get_api()->use_fbo(m_id); }

    ~framebuffer() { render_api_impl::get_api()->delete_renderbuffer(m_id); }

private:
    framebuffer_id m_id;

private:
    framebuffer(const framebuffer &other) = delete;
    framebuffer(framebuffer &&other) = delete;
    framebuffer &operator=(framebuffer &&other) = delete;
    framebuffer &operator=(framebuffer other) = delete;
};

} // namespace blood