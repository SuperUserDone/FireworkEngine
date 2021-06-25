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

namespace blood {

class renderer
{
public:
    renderer(render_settings &p_render_settings);

    bool check_close();

    void render(const scene *scene);
    void render_editor(const scene *scene,
                       glm::mat4 camera_view,
                       glm::mat4 camera_proj,
                       std::function<bool(void)> draw_ui,
                       std::function<glm::uvec2(void)> get_size,
                       std::function<void(size_t)> set_tex_id);

    ~renderer();

private:
    void renderpass_geom(const scene *scene, glm::mat4 camera_view, glm::mat4 camera_proj);
    void renderpass_postfx(texture2d &src, framebuffer_id dest, shader_program &shader);

    buffer m_camera;

    component_mesh m_quad_mesh;

    shader_program m_fullscreen_shader;
    shader_program m_outline_shader;

    framebuffer_screen m_fbo;

    glm::uvec2 m_screen_size;

    render_settings &m_settings;

private:
    renderer() = delete;
    renderer(const renderer &other) = delete;
    renderer &operator=(renderer &&other) = delete;
    renderer &operator=(renderer other) = delete;
};

} // namespace blood
