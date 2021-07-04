#pragma once

#include "render/render_api/render_api.hpp"

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

    shader_program_id get_id() { return m_id; }

    void compile()
    {
        shader_id vert = render_api_impl::get_api()->alloc_shader(SHADER_VERTEX);
        shader_id frag = render_api_impl::get_api()->alloc_shader(SHADER_FRAGMENT);

        render_api_impl::get_api()->compile_shader(vert, m_vert_shader_src.c_str());
        render_api_impl::get_api()->compile_shader(frag, m_frag_shader_src.c_str());

        render_api_impl::get_api()->add_shader_to_program(m_id, vert);
        render_api_impl::get_api()->add_shader_to_program(m_id, frag);

        render_api_impl::get_api()->link_shader_program(m_id);

        render_api_impl::get_api()->delete_shader(frag);
        render_api_impl::get_api()->delete_shader(vert);
    }

    ~shader_program() { render_api_impl::get_api()->delete_shader_program(m_id); }

private:
    shader_program_id m_id;

    std::string m_vert_shader_src;
    std::string m_frag_shader_src;
};
} // namespace fw