#define GLAD_GL_IMPLEMENTATION

#include "core/logger.hpp"
#include "render_api/render_api.hpp"
#include "render_data/builtin_primitives.hpp"
#include "render_data/builtin_shaders.hpp"
#include "renderer.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"

#include <Tracy.hpp>

#define rapi blood::render_api_impl::get_api()

namespace blood {

renderer::renderer(render_settings &p_render_settings)
    : m_settings(p_render_settings), m_camera(128, nullptr, MODE_DYNAMIC), m_fbo({800, 600})
{
    m_quad_mesh.verticies = quad_vertex_data;
    m_quad_mesh.indicies = quad_index_data;

    m_quad_mesh.update();

    m_fullscreen_shader.set_source_vertex(vertex_screen);
    m_fullscreen_shader.set_source_fragment(fragment_screen);

    m_fullscreen_shader.compile();

    m_outline_shader.set_source_vertex(vertex_std);
    m_outline_shader.set_source_fragment(fragment_outline);

    m_outline_shader.compile();

    m_camera.bind_uniform(0);
}

bool renderer::check_close() { return rapi->check_close(); }

void renderer::render(const scene *scene)
{
    rapi->begin(glm::vec4(scene->m_back_color, 1.f));

    auto size = rapi->getsize();
    m_fbo.resize(size);
    m_fbo.get_fbo().use();

    m_screen_size = size;

    glm::mat4 view;
    glm::mat4 proj;

    auto cams = scene->m_entt.view<component_camera, component_transform>();
    for (auto c : cams) {
        auto [cam, trans] = cams.get(c);
        proj = cam.get_projection(m_screen_size.x, m_screen_size.y);
        view = glm::inverse((glm::mat4)trans);
    }

    renderpass_geom(scene, view, proj);
    renderpass_postfx(m_fbo.get_texture(), &DEFAULT_FRAMEBUFFER, m_fullscreen_shader);

    rapi->end();
    FrameMark;
}

void renderer::render_editor(const scene *scene,
                             glm::mat4 camera_view,
                             glm::mat4 camera_proj,
                             std::function<bool(void)> draw_ui,
                             std::function<glm::uvec2(void)> get_size,
                             std::function<void(size_t)> set_tex_id)
{
    rapi->begin();

    glm::uvec2 size = get_size();
    set_tex_id(*(uint32_t *)m_fbo.get_texture().get_id());

    m_screen_size = size;

    rapi->viewport(size);

    m_fbo.resize(size);
    m_fbo.get_fbo().use();

    renderpass_geom(scene, camera_view, camera_proj);
    renderpass_postfx(m_fbo.get_texture(), m_fbo.get_fbo().get_id(), m_fullscreen_shader);

    size = rapi->getsize();
    rapi->viewport(size);

    rapi->use_fbo(&DEFAULT_FRAMEBUFFER);

    rapi->draw_imgui(draw_ui);

    rapi->end();
    FrameMark;
}

void renderer::renderpass_geom(const scene *scene, glm::mat4 camera_view, glm::mat4 camera_proj)
{
    ZoneScopedN("Geometry pass");
    rapi->clear(glm::vec4(scene->m_back_color, 1.f));

    glm::mat4 arr[2];

    arr[0] = camera_proj;
    arr[1] = camera_view;

    m_camera.set_data(128, arr, MODE_DYNAMIC);

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
}

void renderer::renderpass_postfx(texture2d &src, framebuffer_id dest, shader_program &shader)
{
    ZoneScopedN("FX Pass");
    rapi->use_fbo(dest);

    rapi->draw_elements(m_quad_mesh.m_vao.get_id(),
                        m_quad_mesh.m_index_buf.get_id(),
                        m_quad_mesh.indicies.size(),
                        glm::mat4(1.f),
                        shader.get_id(),
                        {src.get_id()});
}

renderer::~renderer() {}

} // namespace blood