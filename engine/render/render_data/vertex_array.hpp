#pragma once

#include "buffer.hpp"
#include "render/render_api/render_api.hpp"

namespace blood {

class vertex_array
{
public:
    vertex_array() { m_id = render_api_impl::get_api()->alloc_vertexarray(); }

    vertex_array(vertex_array &&other) { *this = std::move(other); }
    vertex_array &operator=(vertex_array &&other)
    {
        if (this != &other) {
            m_id = other.m_id;

            other.m_id = nullptr;
        }
        return *this;
    }

    void add_buffer(const buffer &buf, const std::vector<vao_element> &layout)
    {
        render_api_impl::get_api()->set_vertexarray_buffer(m_id, buf.get_id(), layout);
    }

    vao_id get_id() const { return m_id; }

    ~vertex_array()
    {
        if (m_id != nullptr) render_api_impl::get_api()->delete_vertexarray(m_id);
    }

private:
    vao_id m_id;

private:
    vertex_array(const vertex_array &other) = delete;
    vertex_array &operator=(const vertex_array &other) = delete;
};

} // namespace blood