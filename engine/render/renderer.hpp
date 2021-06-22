#pragma once

#include "core/settings.hpp"
#include "render_data/framebuffer.hpp"
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
    void render_editor(const scene *scene);

    ~renderer();

private:
    glm::uvec2 m_old_size;

    buffer m_camera;

    component_mesh m_quad_mesh;

    shader_program m_fullscreen_shader;
    renderbuffer m_depth_stencil;
    texture2d m_color_texture;
    framebuffer m_draw_fbo;

    render_settings &m_settings;

private:
    renderer() = delete;
    renderer(const renderer &other) = delete;
    renderer &operator=(renderer &&other) = delete;
    renderer &operator=(renderer other) = delete;
};

} // namespace blood
