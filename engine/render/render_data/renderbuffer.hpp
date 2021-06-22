#pragma once

#include "render/render_api/render_api.hpp"

namespace blood {

class renderbuffer
{
public:
    renderbuffer(int x, int y)
    {
        m_id = render_api_impl::get_api()->alloc_renderbuffer(x, x, RENDERBUFFER_DEPTH24_STENCIL8);
    }

    void resize(int x, int y)
    {
        if (m_id != nullptr) render_api_impl::get_api()->delete_renderbuffer(m_id);
        m_id = render_api_impl::get_api()->alloc_renderbuffer(x, x, RENDERBUFFER_DEPTH24_STENCIL8);
    }
    renderbuffer_id get_id() const { return m_id; }

    ~renderbuffer() { render_api_impl::get_api()->delete_renderbuffer(m_id); }

private:
    renderbuffer_id m_id;

private:
    renderbuffer(const renderbuffer &other) = delete;
    renderbuffer(renderbuffer &&other) = delete;
    renderbuffer &operator=(renderbuffer &&other) = delete;
    renderbuffer &operator=(renderbuffer other) = delete;
};

}; // namespace blood