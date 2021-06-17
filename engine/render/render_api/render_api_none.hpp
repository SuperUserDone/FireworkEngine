#pragma once

#include "core/settings.hpp"
#include "render/datatypes.hpp"

#include <glm/glm.hpp>
#include <stdint.h>
#include <vector>

namespace blood {

enum alloc_mode : uint8_t
{
    MODE_DYNAMIC,
    MODE_STATIC
};

enum shader_type
{
    SHADER_VERTEX,
    SHADER_FRAGMENT,
};

enum color_format
{
    FORMAT_R,
    FORMAT_RG,
    FORMAT_RGB,
    FORMAT_RGBA,
};

struct vao_element
{
    layout_types type;
    uint16_t vao_attrib = 0;
    uint16_t size = 0;
    uint16_t offset = 0;
    uint16_t stride = 0;
    bool normalized = false;
};

typedef void *buffer_id;
typedef void *vao_id;
typedef void *framebuffer_id;
typedef void *shader_id;
typedef void *shader_program_id;
typedef void *texture_id;

class render_api
{
public:
    render_api() {}
    render_api(const render_settings m_settings) {}

    virtual framebuffer_id alloc_framebuffer() { return nullptr; }

    virtual renderer_impl get_impl() { return RENDERER_NONE; }
    virtual bool check_close() { return m_close; }

    virtual void clear_fb(framebuffer_id id) {}
    virtual void delete_framebuffer(framebuffer_id id) {}

    virtual void begin(glm::vec4 color = {0, 0, 0, 1}) {}

    virtual buffer_id alloc_buffer(size_t size,
                                   const void *data = nullptr,
                                   alloc_mode mode = MODE_STATIC)
    {
        return nullptr;
    }
    virtual void set_buffer_data(buffer_id buf,
                                 size_t size,
                                 const void *data = nullptr,
                                 alloc_mode mode = MODE_STATIC)
    {}
    virtual void bind_as_uniformbuffer(buffer_id id, uint32_t binding) {}
    virtual void delete_buffer(buffer_id buf) {}

    virtual vao_id alloc_vertexarray() { return nullptr; }
    virtual void set_vertexarray_buffer(vao_id varr,
                                        const buffer_id &buf,
                                        const std::vector<vao_element> &layout)
    {}
    virtual void delete_vertexarray(vao_id varr) {}

    virtual shader_id alloc_shader(shader_type type) { return nullptr; }
    virtual bool compile_shader(shader_id id, const char *source) { return true; }
    virtual void delete_shader(shader_id id) {}

    virtual shader_program_id alloc_shader_program() { return nullptr; }
    virtual void add_shader_to_program(const shader_program_id id, const shader_id shader) {}
    virtual bool link_shader_program(const shader_program_id id) { return true; }
    virtual void delete_shader_program(const shader_program_id id) {}

    virtual texture_id alloc_texture() { return nullptr; }
    virtual void set_texture2d_data(
        texture_id id, uint32_t x, uint32_t y, const color_format, const void *data)
    {}
    virtual void delete_texture2d(texture_id id) {}

    virtual void draw_vertexarray(vao_id varr,
                                  uint32_t triangle_count,
                                  const glm::mat4 &transform,
                                  shader_program_id shader,
                                  const std::vector<texture_id> &textures) const
    {}
    virtual void draw_elements(vao_id varr,
                               buffer_id index_buf,
                               uint32_t triangle_count,
                               const glm::mat4 &transform,
                               shader_program_id shader,
                               const std::vector<texture_id> &textures) const
    {}

    virtual void end() {}

    virtual glm::uvec2 getsize() {}

    virtual ~render_api() {}

protected:
    bool m_close = false;
};

} // namespace blood