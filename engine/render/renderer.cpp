#include "renderer.hpp"

#define GLAD_GL_IMPLEMENTATION

#include <glad/gl.h>

#include "core/logger.hpp"

namespace blood
{
renderer::renderer(const render_settings &p_render_settings) : m_settings(p_render_settings)
{
    create_window();
    create_gl_context();
}

void renderer::process_events()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            m_should_close = true;
            break;

        default:
            break;
        }
    }
}

void renderer::render()
{
    glClearColor(0.2, 0.3, 0.5, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    SDL_GL_SwapWindow(m_window);
}

renderer::~renderer()
{
    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void renderer::create_window()
{
    m_window = nullptr;

    BLOODENGINE_ASSERT(SDL_Init(SDL_INIT_VIDEO) == 0, "COULD NOT INIT SDL");

    LOG_IF("Creating window with resolution {}x{} maximized on monitor {}, with vsync = {}",
           m_settings.width,
           m_settings.height,
           m_settings.monitor,
           m_settings.vsync);

    m_window = SDL_CreateWindow("Blood",
                                SDL_WINDOWPOS_CENTERED_DISPLAY(m_settings.monitor),
                                SDL_WINDOWPOS_CENTERED_DISPLAY(m_settings.monitor),
                                m_settings.width,
                                m_settings.height,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);

    if (!m_window)
    {
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR, "COULD NOT INIT SDL", "ERROR CREATING WINDOW", m_window);
        LOG_RUNTIME_ERROR("COULD NOT INIT SDL");
    }
}

void renderer::create_gl_context()
{
    LOG_I("Creating OpenGL context.");

    m_context = SDL_GL_CreateContext(m_window);
    if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "COULD NOT LOAD OPENGL", "ERROR", m_window);
        LOG_RUNTIME_ERROR("COULD NOT LOAD OPENGL");
    }
    SDL_GL_SetSwapInterval(m_settings.vsync);
}
} // namespace blood