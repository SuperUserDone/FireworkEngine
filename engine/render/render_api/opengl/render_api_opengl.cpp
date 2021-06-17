#include "render_api_opengl.hpp"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

static GLuint to_gl_type(blood::alloc_mode mode)
{
    switch (mode) {
    case blood::MODE_DYNAMIC:
        return GL_DYNAMIC_DRAW;
        break;
    case blood::MODE_STATIC:
        return GL_STATIC_DRAW;
        break;
    }

    BLOODENGINE_ASSERT(false, "Not implemented");
    return 0;
}

static GLuint to_gl_type(blood::shader_type type)
{
    switch (type) {
    case blood::SHADER_VERTEX:
        return GL_VERTEX_SHADER;
        break;
    case blood::SHADER_FRAGMENT:
        return GL_FRAGMENT_SHADER;
        break;
    }

    BLOODENGINE_ASSERT(false, "Not implemented");
    return 0;
}

static GLuint to_gl_type(blood::layout_types type)
{
    switch (type) {
    case blood::LAYOUT_FLOAT:
    case blood::LAYOUT_FLOAT2:
    case blood::LAYOUT_FLOAT3:
    case blood::LAYOUT_FLOAT4:
    case blood::LAYOUT_FLOAT_MAT4:
        return GL_FLOAT;
        break;
    case blood::LAYOUT_INT:
    case blood::LAYOUT_INT2:
    case blood::LAYOUT_INT3:
    case blood::LAYOUT_INT4:
        return GL_INT;
        break;
    case blood::LAYOUT_UINT:
    case blood::LAYOUT_UINT2:
    case blood::LAYOUT_UINT3:
    case blood::LAYOUT_UINT4:
        GL_UNSIGNED_INT;
        break;
    }

    BLOODENGINE_ASSERT(false, "Not implemented");
    return 0;
}

namespace blood {
render_api_opengl::render_api_opengl(const render_settings &settings)
    : m_settings(settings), m_win(settings)
{
    glEnable(GL_DEPTH_TEST);

    auto a = R"(
#version 450 core
layout(location = 0) in vec3 aPos;
layout(std140, binding = 0) uniform Matrices{
mat4 projection;
mat4 view;
};
layout(location = 0) uniform mat4 model;
out vec4 vertexColor;
void main() { gl_Position = projection * view * model * vec4(aPos, 1.0); vertexColor = vec4(0.5, 0.0, 0.0, 1.0); }
)";
    auto b = R"(
#version 450 core
out vec4 FragColor;
in vec4 vertexColor;
void main() { FragColor = vertexColor; }
)";

    auto vs = alloc_shader(SHADER_VERTEX);
    auto fs = alloc_shader(SHADER_FRAGMENT);
    m_error_shader = alloc_shader_program();

    compile_shader(vs, a);
    compile_shader(fs, b);
    add_shader_to_program(m_error_shader, vs);
    add_shader_to_program(m_error_shader, fs);
    link_shader_program(m_error_shader);

    delete_shader(vs);
    delete_shader(fs);
}

framebuffer_id render_api_opengl::alloc_framebuffer()
{
    uint32_t *id = new uint32_t;

    glCreateFramebuffers(1, id);

    return id;
}

void render_api_opengl::clear_fb(framebuffer_id id)
{
    // TODO
}

void render_api_opengl::delete_framebuffer(framebuffer_id id)
{
    glDeleteFramebuffers(1, (uint32_t *)id);
    delete (uint32_t *)id;
}

void render_api_opengl::begin(glm::vec4 color)
{
    auto size = m_win.getsize();

    glViewport(0, 0, size.x, size.y);

    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

buffer_id render_api_opengl::alloc_buffer(size_t size, const void *data, alloc_mode mode)
{
    uint32_t *id = new uint32_t;
    glCreateBuffers(1, id);
    glNamedBufferData(*id, size, data, to_gl_type(mode));

    return id;
}

void render_api_opengl::set_buffer_data(buffer_id buf,
                                        size_t size,
                                        const void *data,
                                        alloc_mode mode)
{
    glNamedBufferData(*(uint32_t *)buf, size, data, to_gl_type(mode));
}

void render_api_opengl::bind_as_uniformbuffer(buffer_id id, uint32_t binding)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, *(uint32_t *)id);
}

void render_api_opengl::delete_buffer(buffer_id buf)
{
    glDeleteBuffers(1, (uint32_t *)buf);
    delete (uint32_t *)buf;
}

vao_id render_api_opengl::alloc_vertexarray()
{
    uint32_t *id = new uint32_t;

    glCreateVertexArrays(1, id);

    return id;
}

void render_api_opengl::set_vertexarray_buffer(vao_id varr,
                                               const buffer_id &buf,
                                               const std::vector<vao_element> &layout)
{
    glBindVertexArray(*(uint32_t *)varr);
    glBindBuffer(GL_ARRAY_BUFFER, *(uint32_t *)buf);

    for (auto &&el : layout) {
        glEnableVertexAttribArray(el.vao_attrib);
        glBindVertexBuffer(el.vao_attrib, *(uint32_t *)buf, el.offset, el.stride);
        glVertexAttribPointer(el.vao_attrib,
                              get_datatype_size(el.type),
                              to_gl_type(el.type),
                              el.normalized ? GL_TRUE : GL_FALSE,
                              el.stride,
                              (void *)static_cast<size_t>(el.offset));
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void render_api_opengl::delete_vertexarray(vao_id varr)
{
    glDeleteVertexArrays(1, (uint32_t *)varr);

    delete (uint32_t *)varr;
}

shader_id render_api_opengl::alloc_shader(shader_type type)
{
    uint32_t *id = new uint32_t;

    *id = glCreateShader(to_gl_type(type));

    return id;
}

bool render_api_opengl::compile_shader(shader_id id, const char *source)
{
    glShaderSource(*(uint32_t *)id, 1, &source, nullptr);
    glCompileShader(*(uint32_t *)id);

    int success;
    char shader_log[512];
    glGetShaderiv(*(uint32_t *)id, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(*(uint32_t *)id, 512, NULL, shader_log);
        LOG_EF("Could not compile shader:\n {}", shader_log)
        return false;
    } else {
        return true;
    }
}

void render_api_opengl::delete_shader(shader_id id)
{
    glDeleteShader(*(uint32_t *)id);
    delete (uint32_t *)id;
}

shader_program_id render_api_opengl::alloc_shader_program()
{
    uint32_t *id = new uint32_t;

    *id = glCreateProgram();

    return id;
}

void render_api_opengl::add_shader_to_program(const shader_program_id id, const shader_id shader)
{
    glAttachShader(*(uint32_t *)id, *(uint32_t *)shader);
}

bool render_api_opengl::link_shader_program(const shader_program_id id)
{
    glLinkProgram(*(uint32_t *)id);

    int success;
    char shader_log[512];
    glGetProgramiv(*(uint32_t *)id, GL_LINK_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(*(uint32_t *)id, 512, NULL, shader_log);
        LOG_EF("Could not link shader program:\n {}", shader_log)
        return false;
    } else {
        return true;
    }
}

void render_api_opengl::delete_shader_program(const shader_program_id id)
{
    glDeleteProgram(*(uint32_t *)id);
    delete (uint32_t *)id;
}

texture_id render_api_opengl::alloc_texture()
{ // TODO
    return nullptr;
}

void render_api_opengl::set_texture2d_data(
    texture_id id, uint32_t x, uint32_t y, const color_format, const void *data)
{
    // TODO
}

void render_api_opengl::delete_texture2d(texture_id id)
{
    // TODO
}

void render_api_opengl::draw_vertexarray(vao_id varr,
                                         uint32_t triangle_count,
                                         const glm::mat4 &transform,
                                         shader_program_id shader,
                                         const std::vector<texture_id> &textures) const
{
    if (shader) glUseProgram(*(uint32_t *)shader);
    else
        glUseProgram(*(uint32_t *)m_error_shader);

    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(transform));

    glBindVertexArray(*(uint32_t *)varr);
    glDrawArrays(GL_TRIANGLES, 0, triangle_count);
    glBindVertexArray(0);
}

void render_api_opengl::draw_elements(vao_id varr,
                                      buffer_id index_buf,
                                      uint32_t triangle_count,
                                      const glm::mat4 &transform,
                                      shader_program_id shader,
                                      const std::vector<texture_id> &textures) const
{
    if (shader) glUseProgram(*(uint32_t *)shader);
    else
        glUseProgram(*(uint32_t *)m_error_shader);

    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(transform));

    glBindVertexArray(*(uint32_t *)varr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(uint32_t *)index_buf);
    glDrawElements(GL_TRIANGLES, triangle_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void render_api_opengl::end()
{
    glFlush();

    m_win.process_events();
    m_close = m_win.check_close();
    m_win.swap_buffers();
}

glm::uvec2 render_api_opengl::getsize() { return m_win.getsize(); }

render_api_opengl::~render_api_opengl() { delete_shader_program(m_error_shader); }

} // namespace blood