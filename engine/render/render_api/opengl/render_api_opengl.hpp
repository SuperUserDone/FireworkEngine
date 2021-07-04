#pragma once

#include "../render_api_none.hpp"
#include "opengl_window.hpp"

namespace fw {
class render_api_opengl : public render_api
{
public:
    render_api_opengl(render_settings &settings);

    renderer_impl get_impl() override { return RENDERER_OPENGL33; }

    framebuffer_id alloc_framebuffer() override;
    void set_fbo_color(framebuffer_id id, const std::vector<texture_id> &color) override;
    void set_fbo_depth_stencil_texture(framebuffer_id id, texture_id tex) override;
    void set_fbo_depth_stencil_renderbuffer(framebuffer_id id, renderbuffer_id rb) override;
    void use_fbo(framebuffer_id id) override;
    void delete_framebuffer(framebuffer_id id) override;

    renderbuffer_id alloc_renderbuffer(int x, int y, renderbuffer_format use) override;
    void delete_renderbuffer(renderbuffer_id id) override;

    void clear(glm::vec4 color) override;

    void begin(glm::vec4 color = {0, 0, 0, 1}) override;

    buffer_id alloc_buffer(size_t size = 0,
                           const void *data = nullptr,
                           alloc_mode mode = MODE_STATIC) override;
    void set_buffer_data(buffer_id buf,
                         size_t size,
                         const void *data = nullptr,
                         alloc_mode mode = MODE_STATIC) override;
    void bind_as_uniformbuffer(buffer_id id, uint32_t binding) override;
    void delete_buffer(buffer_id buf) override;

    vao_id alloc_vertexarray() override;
    void set_vertexarray_buffer(vao_id varr,
                                const buffer_id &buf,
                                const std::vector<vao_element> &layout) override;
    void delete_vertexarray(vao_id varr) override;

    shader_id alloc_shader(shader_type type) override;
    bool compile_shader(shader_id id, const char *source) override;
    void delete_shader(shader_id id) override;

    shader_program_id alloc_shader_program() override;
    void add_shader_to_program(const shader_program_id id, const shader_id shader) override;
    bool link_shader_program(const shader_program_id id) override;
    void delete_shader_program(const shader_program_id id) override;

    texture_id alloc_texture2d() override;
    void set_texture2d_data(texture_id id,
                            uint32_t x,
                            uint32_t y,
                            const color_format,
                            const void *data,
                            texture_properties props) override;
    void delete_texture2d(texture_id id) override;

    void draw_vertexarray(vao_id varr,
                          uint32_t triangle_count,
                          const glm::mat4 &transform,
                          shader_program_id shader,
                          const std::vector<texture_id> &textures) const override;
    void draw_elements(vao_id varr,
                       buffer_id index_buf,
                       uint32_t triangle_count,
                       const glm::mat4 &transform,
                       shader_program_id shader,
                       const std::vector<texture_id> &textures) const override;

    void end() override;

    void set_depth_mode(depth_test_mode mode) const override;
    void set_depth_mask(bool mode) const override;

    void draw_imgui(std::function<bool(void)> func) const override;

    void viewport(glm::uvec2 size) override;

    glm::uvec2 getsize() override;

    ~render_api_opengl() override;

private:
    shader_program_id m_error_shader;
    render_settings &m_settings;

    opengl_window m_win;
};

} // namespace fw