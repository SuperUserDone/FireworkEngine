#include "render/render_data/builtin_shaders.hpp"
#include "render_api_opengl.hpp"

#define GLAD_GL_IMPLEMENTATION

#include <Tracy.hpp>
#include <TracyOpenGL.hpp>
#include <glad/gl.h>

static GLuint to_gl_type(fw::alloc_mode mode)
{
    switch (mode) {
    case fw::MODE_DYNAMIC:
        return GL_DYNAMIC_DRAW;
        break;
    case fw::MODE_STATIC:
        return GL_STATIC_DRAW;
        break;
    }

    FIREWORK_ASSERT(false, "Not implemented");
    return 0;
}

static GLuint to_gl_type(fw::shader_type type)
{
    switch (type) {
    case fw::SHADER_VERTEX:
        return GL_VERTEX_SHADER;
        break;
    case fw::SHADER_FRAGMENT:
        return GL_FRAGMENT_SHADER;
        break;
    }

    FIREWORK_ASSERT(false, "Not implemented");
    return 0;
}

static GLuint to_gl_type(fw::layout_types type)
{
    switch (type) {
    case fw::LAYOUT_FLOAT:
    case fw::LAYOUT_FLOAT2:
    case fw::LAYOUT_FLOAT3:
    case fw::LAYOUT_FLOAT4:
    case fw::LAYOUT_FLOAT_MAT4:
        return GL_FLOAT;
        break;
    case fw::LAYOUT_INT:
    case fw::LAYOUT_INT2:
    case fw::LAYOUT_INT3:
    case fw::LAYOUT_INT4:
        return GL_INT;
        break;
    case fw::LAYOUT_UINT:
    case fw::LAYOUT_UINT2:
    case fw::LAYOUT_UINT3:
    case fw::LAYOUT_UINT4:
        GL_UNSIGNED_INT;
        break;
    }

    FIREWORK_ASSERT(false, "Not implemented");
    return 0;
}

static GLuint to_gl_type(fw::color_format fil)
{
    switch (fil) {
    case fw::FORMAT_R:
        return GL_RED;
        break;
    case fw::FORMAT_RG:
        return GL_RG;
        break;
    case fw::FORMAT_RGB:
        return GL_RGB;
        break;
    case fw::FORMAT_RGBA:
        return GL_RGBA;
        break;
    case fw::FORMAT_SRGB:
        return GL_SRGB;
        break;
    case fw::FORMAT_SRGBA:
        return GL_SRGB_ALPHA;
        break;
    case fw::FORMAT_DEPTH24_STENCIL8:
        return GL_DEPTH24_STENCIL8;
        break;
    case fw::FORMAT_DEPTH24:
        return GL_DEPTH_COMPONENT24;
        break;
    }

    FIREWORK_ASSERT(false, "Not implemented");
    return 0;
}

static GLuint to_gl_type_internal(fw::color_format fil)
{
    switch (fil) {
    case fw::FORMAT_R:
        return GL_RED;
        break;
    case fw::FORMAT_RG:
        return GL_RG;
        break;
    case fw::FORMAT_RGB:
    case fw::FORMAT_SRGB:
        return GL_RGB;
        break;
    case fw::FORMAT_RGBA:
    case fw::FORMAT_SRGBA:
        return GL_RGBA;
        break;
    case fw::FORMAT_DEPTH24_STENCIL8:
        return GL_DEPTH24_STENCIL8;
        break;
    case fw::FORMAT_DEPTH24:
        return GL_DEPTH_COMPONENT24;
        break;
    }

    FIREWORK_ASSERT(false, "Not implemented");
    return 0;
}

static GLuint to_gl_type(fw::renderbuffer_format format)
{
    switch (format) {
    case fw::RENDERBUFFER_DEPTH24_STENCIL8:
        return GL_DEPTH24_STENCIL8;
        break;
    case fw::RENDERBUFFER_DEPTH24:
        return GL_DEPTH_COMPONENT24;
        break;
    }
    FIREWORK_ASSERT(false, "Not implemented");
    return 0;
}

static GLuint to_gl_type(fw::texture_filter filter, bool mipmap = false)
{
    switch (filter) {
    case fw::FILTER_LINEAR:
        return mipmap ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR;
    case fw::FILTER_NEAREST:
        return mipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
    }

    FIREWORK_ASSERT(false, "Not implemented");
    return 0;
}

static GLuint to_gl_type(fw::depth_test_mode mode)
{
    switch (mode) {
    case fw::TEST_ALWAYS:
        return GL_ALWAYS;
        break;
    case fw::TEST_LESS:
        return GL_LESS;
        break;
    case fw::TEST_GREATER:
        return GL_GREATER;
        break;
    case fw::TEST_EQUAL:
        return GL_EQUAL;
        break;
    case fw::TEST_GEQUAL:
        return GL_GEQUAL;
        break;
    case fw::TEST_LEQUAL:
        return GL_LEQUAL;
        break;
    case fw::TEST_NEVER:
        return GL_NEVER;
        break;
    case fw::TEST_NOTEQUAL:
        return GL_NOTEQUAL;
        break;
    }

    FIREWORK_ASSERT(false, "Not implemented");
    return 0;
}

namespace fw {
render_api_opengl::render_api_opengl(render_settings &settings)
    : m_settings(settings), m_win(settings)
{
    glEnable(GL_DEPTH_TEST);

    auto vs = alloc_shader(SHADER_VERTEX);
    auto fs = alloc_shader(SHADER_FRAGMENT);
    m_error_shader = alloc_shader_program();

    compile_shader(vs, vertex_std);
    compile_shader(fs, fragment_err);
    add_shader_to_program(m_error_shader, vs);
    add_shader_to_program(m_error_shader, fs);
    link_shader_program(m_error_shader);

    delete_shader(vs);
    delete_shader(fs);
}

void render_api_opengl::set_srgb(bool mode)
{
    if (mode)
        glEnable(GL_FRAMEBUFFER_SRGB);
    else
        glDisable(GL_FRAMEBUFFER_SRGB);
}

framebuffer_id render_api_opengl::alloc_framebuffer()
{
    uint32_t *id = new uint32_t;

    TracyGpuZone("Alloc Framebuffer");
    glCreateFramebuffers(1, id);

    return id;
}

void render_api_opengl::set_fbo_color(framebuffer_id id, const std::vector<texture_id> &color)
{
    for (int i = 0; i < color.size(); i++) {
        TracyGpuZone("Set Framebuffer Color Attatchment");
        glNamedFramebufferTexture(
            *(uint32_t *)id, GL_COLOR_ATTACHMENT0 + i, *(uint32_t *)color[i], 0);
    }
}

void render_api_opengl::set_fbo_depth_stencil_texture(framebuffer_id id, texture_id tex)
{
    TracyGpuZone("Set Framebuffer depth/stencil Attatchment texture");
    glNamedFramebufferTexture(*(uint32_t *)id, GL_DEPTH_STENCIL_ATTACHMENT, *(uint32_t *)tex, 0);
}

void render_api_opengl::set_fbo_depth_stencil_renderbuffer(framebuffer_id id, renderbuffer_id rb)
{
    TracyGpuZone("Set Framebuffer depth/stencil Attatchment renderbuffer");
    glNamedFramebufferRenderbuffer(
        *(uint32_t *)id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *(uint32_t *)rb);
}

void render_api_opengl::use_fbo(framebuffer_id id)
{
    TracyGpuZone("Bind Framebuffer");
    glBindFramebuffer(GL_FRAMEBUFFER, *(uint32_t *)id);
}

void render_api_opengl::delete_framebuffer(framebuffer_id id)
{
    TracyGpuZone("Delete Framebuffer");
    glDeleteFramebuffers(1, (uint32_t *)id);
    delete (uint32_t *)id;
}

renderbuffer_id render_api_opengl::alloc_renderbuffer(int x, int y, renderbuffer_format use)
{
    TracyGpuZone("Allocate renderbuffer");
    uint32_t *id = new uint32_t;

    glCreateRenderbuffers(1, id);
    glNamedRenderbufferStorage(*id, to_gl_type(use), x, y);

    return id;
}

void render_api_opengl::delete_renderbuffer(renderbuffer_id id)
{
    TracyGpuZone("Delete Renderbuffer");
    glDeleteRenderbuffers(1, (uint32_t *)id);
    delete (uint32_t *)id;
}

void render_api_opengl::clear(glm::vec4 color)
{
    TracyGpuZone("Clear FBO");
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void render_api_opengl::begin(glm::vec4 color)
{
    auto size = m_win.getsize();

    glViewport(0, 0, size.x, size.y);

    clear(color);
}

buffer_id render_api_opengl::alloc_buffer(size_t size, const void *data, alloc_mode mode)
{
    uint32_t *id = new uint32_t;

    TracyGpuZone("Allocate buffer");
    glCreateBuffers(1, id);
    glNamedBufferData(*id, size, data, to_gl_type(mode));

    return id;
}

void render_api_opengl::set_buffer_data(buffer_id buf,
                                        size_t size,
                                        const void *data,
                                        alloc_mode mode)
{
    TracyGpuZone("Fill buffer");
    glNamedBufferData(*(uint32_t *)buf, size, data, to_gl_type(mode));
}

void render_api_opengl::bind_as_uniformbuffer(buffer_id id, uint32_t binding)
{
    TracyGpuZone("Use buffer as UBO");
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, *(uint32_t *)id);
}

void render_api_opengl::delete_buffer(buffer_id buf)
{
    TracyGpuZone("Delete buffer");
    glDeleteBuffers(1, (uint32_t *)buf);
    delete (uint32_t *)buf;
}

vao_id render_api_opengl::alloc_vertexarray()
{
    uint32_t *id = new uint32_t;

    TracyGpuZone("Allocate VAO");
    glCreateVertexArrays(1, id);

    return id;
}

void render_api_opengl::set_vertexarray_buffer(vao_id varr,
                                               const buffer_id &buf,
                                               const std::vector<vao_element> &layout)
{
    {
        TracyGpuZone("Bind VAO");
        glBindVertexArray(*(uint32_t *)varr);
        glBindBuffer(GL_ARRAY_BUFFER, *(uint32_t *)buf);
    }

    for (auto &&el : layout) {
        TracyGpuZone("Set VAO layout");
        glEnableVertexAttribArray(el.vao_attrib);
        glBindVertexBuffer(el.vao_attrib, *(uint32_t *)buf, el.offset, el.stride);
        glVertexAttribPointer(el.vao_attrib,
                              get_datatype_size(el.type),
                              to_gl_type(el.type),
                              el.normalized ? GL_TRUE : GL_FALSE,
                              el.stride,
                              (void *)static_cast<size_t>(el.offset));
    }

    {
        TracyGpuZone("Unbind VAO");
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void render_api_opengl::delete_vertexarray(vao_id varr)
{
    TracyGpuZone("Delete VAO");

    glDeleteVertexArrays(1, (uint32_t *)varr);

    delete (uint32_t *)varr;
}

shader_id render_api_opengl::alloc_shader(shader_type type)
{
    uint32_t *id = new uint32_t;

    TracyGpuZone("Allocate Shader");
    *id = glCreateShader(to_gl_type(type));

    return id;
}

bool render_api_opengl::compile_shader(shader_id id, const char *source)
{
    {
        TracyGpuZone("Set Shader Source");
        glShaderSource(*(uint32_t *)id, 1, &source, nullptr);
    }
    {
        TracyGpuZone("Compile shader");
        glCompileShader(*(uint32_t *)id);
    }

    {
        TracyGpuZone("Get shader compiling status");
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
}

void render_api_opengl::delete_shader(shader_id id)
{
    TracyGpuZone("Delete shader");
    glDeleteShader(*(uint32_t *)id);
    delete (uint32_t *)id;
}

shader_program_id render_api_opengl::alloc_shader_program()
{
    uint32_t *id = new uint32_t;

    TracyGpuZone("Allocate Shader program");
    *id = glCreateProgram();

    return id;
}

void render_api_opengl::add_shader_to_program(const shader_program_id id, const shader_id shader)
{
    TracyGpuZone("Attatch Shader to Shader program");
    glAttachShader(*(uint32_t *)id, *(uint32_t *)shader);
}

bool render_api_opengl::link_shader_program(const shader_program_id id)
{
    {
        TracyGpuZone("Link Shader program");
        glLinkProgram(*(uint32_t *)id);
    }

    {
        TracyGpuZone("Get Shader program status");
        int success;
        char shader_log[512];
        glGetProgramiv(*(uint32_t *)id, GL_LINK_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(*(uint32_t *)id, 512, NULL, shader_log);
            LOG_EF("Could not link shader program:\n {}", shader_log);
            return false;
        } else {
            return true;
        }
    }
}

void render_api_opengl::delete_shader_program(const shader_program_id id)
{
    TracyGpuZone("Delete shader program");
    glDeleteProgram(*(uint32_t *)id);
    delete (uint32_t *)id;
}

texture_id render_api_opengl::alloc_texture2d()
{
    uint32_t *id = new uint32_t;

    TracyGpuZone("Allocate Texture");
    glCreateTextures(GL_TEXTURE_2D, 1, id);

    return id;
}

void render_api_opengl::set_texture2d_data(texture_id id,
                                           uint32_t x,
                                           uint32_t y,
                                           const color_format fmt,
                                           const void *data,
                                           texture_properties props)
{
    TracyGpuZone("Set Texture Data");
    glBindTexture(GL_TEXTURE_2D, *(uint32_t *)id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, to_gl_type(props.min, props.mipmap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, to_gl_type(props.mag));

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 to_gl_type(fmt),
                 x,
                 y,
                 0,
                 to_gl_type_internal(fmt),
                 GL_UNSIGNED_BYTE,
                 data);

    if (props.mipmap) glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void render_api_opengl::delete_texture2d(texture_id id)
{
    TracyGpuZone("Delete Texture");
    glDeleteTextures(1, (uint32_t *)id);
    delete (uint32_t *)id;
}

void render_api_opengl::draw_vertexarray(vao_id varr,
                                         uint32_t triangle_count,
                                         const glm::mat4 &transform,
                                         shader_program_id shader,
                                         const std::vector<texture_id> &textures) const
{
    {
        TracyGpuZone("Bind shader");
        if (shader)
            glUseProgram(*(uint32_t *)shader);
        else
            glUseProgram(*(uint32_t *)m_error_shader);
    }

    {
        TracyGpuZone("Set Uniforms");
        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(transform));
    }

    {
        TracyGpuZone("Draw Call");
        glBindVertexArray(*(uint32_t *)varr);
        glDrawArrays(GL_TRIANGLES, 0, triangle_count);
        glBindVertexArray(0);
    }
}

void render_api_opengl::draw_elements(vao_id varr,
                                      buffer_id index_buf,
                                      uint32_t triangle_count,
                                      const glm::mat4 &transform,
                                      shader_program_id shader,
                                      const std::vector<attribute> &uniforms) const
{
    {
        TracyGpuZone("Bind Shader");
        if (shader)
            glUseProgram(*(uint32_t *)shader);
        else
            glUseProgram(*(uint32_t *)m_error_shader);
    }

    {
        TracyGpuZone("Bind Textures");
        for (auto &uniform : uniforms) {
            switch (uniform.type) {
            case ATTRIB_TYPE_BOOL:
                glUniform1i(uniform.bind_id, uniform.data.d_bool);
                break;
            case ATTRIB_TYPE_INT:
                glUniform1i(uniform.bind_id, uniform.data.d_int);
                break;
            case ATTRIB_TYPE_UINT:
                glUniform1ui(uniform.bind_id, uniform.data.d_uint);
                break;
            case ATTRIB_TYPE_FLOAT:
                glUniform1f(uniform.bind_id, uniform.data.d_float);
                break;
            case ATTRIB_TYPE_DOUBLE:
                glUniform1d(uniform.bind_id, uniform.data.d_double);
                break;
            case ATTRIB_TYPE_TEXTURE:
                glActiveTexture(GL_TEXTURE0 + uniform.bind_id);

                if (uniform.data.d_texture != nullptr)
                    glBindTexture(GL_TEXTURE_2D, *(uint32_t *)uniform.data.d_texture);
                else
                    glUseProgram(*(uint32_t *)m_error_shader);
                break;
            case ATTRIB_TYPE_VEC2F:
                glUniform2f(uniform.bind_id, uniform.data.d_vec2f.x, uniform.data.d_vec2f.y);
                break;
            case ATTRIB_TYPE_VEC2I:
                glUniform2i(uniform.bind_id, uniform.data.d_vec2i.x, uniform.data.d_vec2i.y);
                break;
            case ATTRIB_TYPE_VEC2U:
                glUniform2ui(uniform.bind_id, uniform.data.d_vec2u.x, uniform.data.d_vec2u.y);
                break;
            case ATTRIB_TYPE_VEC3F:
                glUniform3f(uniform.bind_id,
                            uniform.data.d_vec3f.x,
                            uniform.data.d_vec3f.y,
                            uniform.data.d_vec3f.z);
                break;
            case ATTRIB_TYPE_VEC3I:
                glUniform3i(uniform.bind_id,
                            uniform.data.d_vec3i.x,
                            uniform.data.d_vec3i.y,
                            uniform.data.d_vec3i.z);
                break;
            case ATTRIB_TYPE_VEC3U:
                glUniform3ui(uniform.bind_id,
                             uniform.data.d_vec3u.x,
                             uniform.data.d_vec3u.y,
                             uniform.data.d_vec3u.z);
                break;
            case ATTRIB_TYPE_VEC4F:
                glUniform4f(uniform.bind_id,
                            uniform.data.d_vec4f.x,
                            uniform.data.d_vec4f.y,
                            uniform.data.d_vec4f.z,
                            uniform.data.d_vec4f.w);
                break;
            case ATTRIB_TYPE_VEC4I:
                glUniform4i(uniform.bind_id,
                            uniform.data.d_vec4i.x,
                            uniform.data.d_vec4i.y,
                            uniform.data.d_vec4i.z,
                            uniform.data.d_vec4i.w);
                break;
            case ATTRIB_TYPE_VEC4U:
                glUniform4ui(uniform.bind_id,
                             uniform.data.d_vec4u.x,
                             uniform.data.d_vec4u.y,
                             uniform.data.d_vec4u.z,
                             uniform.data.d_vec4u.w);
                break;
            }
        }
    }

    {
        TracyGpuZone("Set Uniforms");
        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(transform));
    }

    {
        TracyGpuZone("Draw Call");
        glBindVertexArray(*(uint32_t *)varr);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(uint32_t *)index_buf);
        glDrawElements(GL_TRIANGLES, triangle_count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void render_api_opengl::end()
{
    {
        TracyGpuZone("Flush");
        glFlush();
    }

    {
        TracyGpuZone("Swap");
        m_win.process_events();
        m_close = m_win.check_close();
        m_win.swap_buffers();
    }
}

void render_api_opengl::set_depth_mode(depth_test_mode mode) const
{
    glDepthFunc(to_gl_type(mode));
}

void render_api_opengl::set_depth_mask(bool mode) const { glDepthMask(mode ? GL_TRUE : GL_FALSE); }

void render_api_opengl::draw_imgui(std::function<bool(void)> func) const
{
    TracyGpuZone("Draw imgui");
    m_win.render_imgui(func);
}

void render_api_opengl::viewport(glm::uvec2 size) { glViewport(0, 0, size.x, size.y); }

glm::uvec2 render_api_opengl::getsize() { return m_win.getsize(); }

render_api_opengl::~render_api_opengl() { delete_shader_program(m_error_shader); }

} // namespace fw