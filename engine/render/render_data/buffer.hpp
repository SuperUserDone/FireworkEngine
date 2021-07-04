#pragma once

#include "render/render_api/render_api.hpp"

namespace fw {

class buffer
{
public:
    buffer() { m_id = render_api_impl::get_api()->alloc_buffer(0, nullptr); }

    buffer(buffer &&other) { *this = std::move(other); }
    buffer &operator=(buffer &&other)
    {
        if (this != &other) {
            m_id = other.m_id;

            other.m_id = nullptr;
        }
        return *this;
    }

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

    ~buffer()
    {
        if (m_id != nullptr) render_api_impl::get_api()->delete_buffer(m_id);
    }

    buffer_id get_id() const { return m_id; }

private:
    buffer_id m_id = nullptr;

private:
    buffer(const buffer &other) = delete;
    buffer &operator=(const buffer &other) = delete;
};

} // namespace fw