#pragma once

#include <SDL.h>
#include <memory>

#include "core/settings.hpp"
#include "deletion_helpers.hpp"
#include "framebuffer.hpp"
#include "scene/component_camera.hpp"
#include "scene/component_mesh.hpp"
#include "scene/component_transform.hpp"
#include "scene/scene.hpp"

#ifdef WIN32
typedef unsigned int uint;
#endif

namespace blood
{

class renderer
{
public:
    renderer(const render_settings &p_render_settings);

    bool check_close() { return m_should_close; }
    void process_events();

    void clear_fb(framebuffer *fb);
    void render_imgui(void *state, std::function<bool()> callback);

    void render(double frametime,
                scene *scene,
                component_camera &cam,
                const glm::mat4 &component_transform,
                framebuffer *fb = &DEFAULT_FRAMEBUFFER);

    void finish_render();

    void clean();

    ~renderer();

private:
    bool m_should_close = false;

    render_settings m_settings;

    SDL_Window *m_window;
    SDL_GLContext m_context;

    uint m_camera_buffer = 0;

    void init_cameras();
    void setup_camera(component_camera &cam, const glm::mat4 &camera_transform, int x, int y);

    void create_window();
    void create_gl_context();
    void init_imgui();

    void render_mesh(component_mesh &mesh, component_transform &trans, component_camera &cam) const;
    void load_mesh(component_mesh &mesh, component_transform &trans) const;
    void update_mesh(component_mesh &mesh, component_transform &trans) const;

    void load_material(std::shared_ptr<material> mat) const;

    void create_framebuffer(framebuffer *fb);
    void update_framebuffer_texture(framebuffer *fb);

private:
    renderer() = delete;
    renderer(const renderer &other) = delete;
    renderer &operator=(renderer &&other) = delete;
    renderer &operator=(renderer other) = delete;
};

} // namespace blood
