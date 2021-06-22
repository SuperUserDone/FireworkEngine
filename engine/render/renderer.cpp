#define GLAD_GL_IMPLEMENTATION

#include "core/logger.hpp"
#include "render_api/render_api.hpp"
#include "renderer.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"

#define rapi blood::render_api_impl::get_api()

namespace blood {

renderer::renderer(render_settings &p_render_settings)
    : m_settings(p_render_settings), m_camera(128, nullptr, MODE_DYNAMIC),
      m_depth_stencil(800, 600), m_color_texture(800, 600, FORMAT_DEPTH24_STENCIL8)
{
    {
        m_quad_mesh.verticies = {{{
                                      1.f,
                                      1.f,
                                      0.0f,
                                  },
                                  glm::vec3(0.f),
                                  glm::vec3(0.f),
                                  {1, 1}},
                                 {
                                     {
                                         1.f,
                                         -1.0f,
                                         0.0f,
                                     },
                                     glm::vec3(0.f),
                                     glm::vec3(0.f),
                                     {1, 0},
                                 },
                                 {
                                     {
                                         -1.f,
                                         -1.f,
                                         0.0f, // bottom left
                                     },
                                     glm::vec3(0.f),
                                     glm::vec3(0.f),
                                     {0, 0},
                                 },
                                 {
                                     {
                                         -1.f,
                                         1.f,
                                         0.0f // top left
                                     },
                                     glm::vec3(0.f),
                                     glm::vec3(0.f),
                                     {0, 1},
                                 }};

        m_quad_mesh.indicies = {0, 1, 3, 1, 2, 3};
        m_quad_mesh.update();
    }

    m_fullscreen_shader.set_source_fragment(
        R"(
#version 450 core
out vec4 FragColor;
in vec2 TexCoords;
layout(binding = 0) uniform sampler2D screenTexture;
void main()
{ 
    FragColor = texture(screenTexture, TexCoords);
}
        )");
    m_fullscreen_shader.set_source_vertex(R"(
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec2 aTexCoords;
out vec2 TexCoords;
void main()
{
    gl_Position = vec4(aPos, 1.0); 
    TexCoords = aTexCoords;
}  
    )");

    m_fullscreen_shader.compile();

    m_camera.bind_uniform(0);

    m_draw_fbo.attach_color({m_color_texture.get_id()});
    m_draw_fbo.attach_depth_stencil_rb(m_depth_stencil);
}

bool renderer::check_close() { return rapi->check_close(); }

void renderer::render(const scene *scene)
{
    rapi->begin(glm::vec4(scene->m_back_color, 1.f));

    auto size = rapi->getsize();

    if (m_old_size != size) {
        m_old_size = size;
        m_depth_stencil.resize(size.x, size.y);
        m_color_texture.set_data(size.x, size.y, nullptr, FORMAT_DEPTH24_STENCIL8);

        m_draw_fbo.attach_color({m_color_texture.get_id()});
        m_draw_fbo.attach_depth_stencil_rb(m_depth_stencil);
    }

    m_draw_fbo.use();
    rapi->clear(glm::vec4(scene->m_back_color, 1.f));

    auto cams = scene->m_entt.view<component_camera, component_transform>();
    for (auto c : cams) {
        auto [cam, trans] = cams.get(c);

        glm::mat4 arr[2];

        arr[0] = cam.get_projection(size.x, size.y);
        arr[1] = glm::inverse((glm::mat4)trans);

        m_camera.set_data(128, arr, MODE_DYNAMIC);
    }

    auto meshes = scene->m_entt.view<component_transform, component_mesh>();

    for (auto c : meshes) {
        auto [trans, mesh] = meshes.get(c);

        rapi->draw_elements(mesh.m_vao.get_id(),
                            mesh.m_index_buf.get_id(),
                            mesh.indicies.size(),
                            trans,
                            nullptr,
                            {});
    }

    rapi->use_fbo(&DEFAULT_FRAMEBUFFER);

    rapi->draw_elements(m_quad_mesh.m_vao.get_id(),
                        m_quad_mesh.m_index_buf.get_id(),
                        m_quad_mesh.indicies.size(),
                        glm::mat4(1.f),
                        m_fullscreen_shader.get_id(),
                        {m_color_texture.get_id()});

    rapi->end();
}

void renderer::render_editor(const scene *scene)
{
    rapi->begin();
    rapi->end();
}

renderer::~renderer() {}

} // namespace blood