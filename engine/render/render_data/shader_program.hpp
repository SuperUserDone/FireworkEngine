#pragma once

#include "render/render_api/render_api.hpp"
#include "render/shader_attribs.hpp"

namespace fw {

class shader_program
{
public:
    shader_program() { m_id = render_api_impl::get_api()->alloc_shader_program(); }
    shader_program(const char *vertex, const char *fragment) : shader_program()
    {
        m_vert_shader_src = vertex;
        m_frag_shader_src = fragment;
    }

    void set_source_vertex(const char *vertex) { m_vert_shader_src = std::string(vertex); }
    void set_source_fragment(const char *fragment) { m_frag_shader_src = std::string(fragment); }

    const std::vector<attribute> get_attribs() const { return m_attribs; }

    shader_program_id get_id() { return m_id; }

    void compile();
    void preprocess(const std::string &src);

    ~shader_program() { render_api_impl::get_api()->delete_shader_program(m_id); }

private:
    shader_program_id m_id;

    std::vector<attribute> m_attribs;

    std::string m_vert_shader_src;
    std::string m_frag_shader_src;
};
} // namespace fw