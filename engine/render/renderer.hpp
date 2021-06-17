#pragma once

#include "core/settings.hpp"
#include "framebuffer.hpp"
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
    renderer(const render_settings &p_render_settings);

    bool check_close();

    void render(const scene *scene);
    void render_editor(const scene *scene);

    ~renderer();

private:
    buffer *m_camera;

private:
    renderer() = delete;
    renderer(const renderer &other) = delete;
    renderer &operator=(renderer &&other) = delete;
    renderer &operator=(renderer other) = delete;
};

} // namespace blood
