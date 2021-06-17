#pragma once

#include "buffer.hpp"
#include "render/render_api/render_api.hpp"

namespace blood {

class vertex_array
{
public:
    vertex_array() { m_id = render_api_impl::get_api()->alloc_vertexarray(); }

    void add_buffer(const buffer &buf, const std::vector<vao_element> &layout)
    {
        render_api_impl::get_api()->set_vertexarray_buffer(m_id, buf.get_id(), layout);
    }

    vao_id get_id() const { return m_id; }

    ~vertex_array() { render_api_impl::get_api()->delete_vertexarray(m_id); }

private:
    vao_id m_id;
};

} // namespace blood