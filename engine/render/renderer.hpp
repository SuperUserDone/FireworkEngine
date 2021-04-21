#pragma once

#include "core/settings.hpp"

#include <SDL2/SDL.h>

namespace blood
{

class renderer
{
public:
    renderer(const render_settings &p_render_settings);

    bool check_close() { return m_should_close; }
    void process_events();
    void render();

    ~renderer();

private:
    bool m_should_close = false;

    render_settings m_settings;

    SDL_Window *m_window;
    SDL_GLContext m_context;

    void create_window();
    void create_gl_context();
};

} // namespace blood
