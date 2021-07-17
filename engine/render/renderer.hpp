#pragma once

#include "core/settings.hpp"
#include "render_data/framebuffer.hpp"
#include "render_data/framebuffer_screen.hpp"
#include "render_data/shader_program.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"

#include <SDL.h>
#include <memory>

#ifdef WIN32
typedef unsigned int uint;
#endif

namespace fw {

class renderer
{
public:
    renderer(render_settings &p_render_settings);

    bool check_close();

    void render(scene *scene);
    void render_editor(scene *scene,
                       glm::mat4 camera_view,
                       glm::mat4 camera_proj,
                       std::function<bool(void)> draw_ui,
                       std::function<glm::uvec2(void)> get_size,
                       std::function<void(size_t)> set_tex_id);

    ~renderer();

private:
    void renderpass_geom(scene *scene, glm::mat4 camera_view, glm::mat4 camera_proj);
    void renderpass_postfx(texture2d &src,
                           framebuffer_id dest,
                           shader_program_id shader,
                           bool srgb);

    void do_lookup(scene *scene, component_mesh &mesh, bool dirty);
    void do_lookup(scene *scene, component_material &mat, bool dirty);

private:
    storage_type<std::string, ref<shader_program>> m_shaders;

    buffer m_camera;

    mesh m_quad_mesh;

    framebuffer_screen m_edit_fbo;
    framebuffer_screen m_fx_fbo;

    glm::uvec2 m_screen_size;

    render_settings &m_settings;

private:
    renderer() = delete;
    renderer(const renderer &other) = delete;
    renderer &operator=(renderer &&other) = delete;
    renderer &operator=(renderer other) = delete;
};

} // namespace fw
