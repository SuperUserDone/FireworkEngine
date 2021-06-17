#pragma once

#include "render/render_api/render_api.hpp"

namespace blood {

class buffer
{
public:
    buffer() { m_id = render_api_impl::get_api()->alloc_buffer(0, nullptr); }

    buffer(const size_t size, const void *data, const alloc_mode mode = MODE_STATIC)
    {
        m_id = render_api_impl::get_api()->alloc_buffer(size, data, mode);
    }

    void set_data(const size_t size, const void *data, const alloc_mode mode = MODE_STATIC)
    {
        render_api_impl::get_api()->set_buffer_data(m_id, size, data, mode);
    }

    void bind_uniform(uint32_t location)
    {
        render_api_impl::get_api()->bind_as_uniformbuffer(m_id, location);
    }

    ~buffer() { render_api_impl::get_api()->delete_buffer(m_id); }

    buffer_id get_id() const { return m_id; }

private:
    buffer_id m_id = nullptr;
};

} // namespace blood