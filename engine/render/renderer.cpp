#include "SDL_video.h"
#include "glm/fwd.hpp"
#include "renderer.hpp"
#include "scene/component_camera.hpp"
#include "scene/component_mesh.hpp"

#define GLAD_GL_IMPLEMENTATION

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/logger.hpp"
#include "scene/scene.hpp"

namespace blood
{
renderer::renderer(const render_settings &p_render_settings) : m_settings(p_render_settings)
{
    create_window();
    create_gl_context();
    init_cameras();
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

void renderer::render(double frametime, std::weak_ptr<scene> scene, component_camera &cam)
{
    // TODO FRAMEBUFFERS

    int x, y;
    SDL_GetWindowSize(m_window, &x, &y);

    glViewport(0, 0, x, y);

    setup_camera(cam, x, y);

    auto scene_ptr = scene.lock();

    glm::vec3 color = scene_ptr->m_back_color;
    glClearColor(color.r, color.g, color.b, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    entt::registry &reg = scene_ptr->m_entt;

    auto view = reg.view<component_mesh, component_transform>();

    for (auto entity : view)
    {
        auto [mesh_comp, trans_comp] = view.get(entity);

        render_mesh(mesh_comp, trans_comp, cam);
    }

    SDL_GL_SwapWindow(m_window);
}

renderer::~renderer()
{
    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void renderer::init_cameras()
{
    glGenBuffers(1, &m_camera_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, m_camera_buffer);
    glBufferData(GL_UNIFORM_BUFFER, 128, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void renderer::setup_camera(component_camera &cam, int x, int y)
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_camera_buffer);
    glBufferSubData(
        GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(cam.get_projection(x, y)));
    glBufferSubData(
        GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr((glm::mat4)cam));

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_camera_buffer, 0, sizeof(glm::mat4) * 2);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
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

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

    m_context = SDL_GL_CreateContext(m_window);
    if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "COULD NOT LOAD OPENGL", "ERROR", m_window);
        LOG_RUNTIME_ERROR("COULD NOT LOAD OPENGL");
    }
    SDL_GL_SetSwapInterval(m_settings.vsync);
}

void renderer::render_mesh(component_mesh &mesh,
                           component_transform &trans,
                           component_camera &cam) const
{
    if (mesh.render_data.vao == -1 || mesh.changed)
        load_mesh(mesh, trans);

    if (mesh.m_mat->shader.render_data.shader_program == -1)
        load_material(mesh.m_mat);

    glUseProgram(mesh.m_mat->shader.render_data.shader_program);

    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr((glm::mat4)trans));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.render_data.index_buffer);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
}

void renderer::load_mesh(component_mesh &mesh, component_transform &trans) const
{
    uint vao;
    uint vbo;
    uint ebo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER,
                 mesh.verticies.size() * sizeof(vertex),
                 mesh.verticies.data(),
                 GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(uint32_t) * mesh.indicies.size(),
                 mesh.indicies.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(vertex), (void *)offsetof(vertex, pos));
    glEnableVertexAttribArray(0);

    mesh.changed = false;
    mesh.render_data.index_buffer = ebo;
    mesh.render_data.vertex_buffer = vbo;
    mesh.render_data.vao = vao;
}

void renderer::load_material(std::shared_ptr<material> mat) const
{
    uint vert_shader, frag_shader, shader_program;

    vert_shader = glCreateShader(GL_VERTEX_SHADER);
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vert_shader, 1, &mat->shader.vert_source, nullptr);
    glShaderSource(frag_shader, 1, &mat->shader.frag_source, nullptr);

    glCompileShader(vert_shader);
    glCompileShader(frag_shader);

    int success;
    char infoLog[512];
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vert_shader, 512, NULL, infoLog);
        LOG_EF("Could not compile vertex shader: {}", infoLog);
    }

    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(frag_shader, 512, NULL, infoLog);
        LOG_EF("Could not compile fragment shader: {}", infoLog);
    }

    shader_program = glCreateProgram();

    glAttachShader(shader_program, vert_shader);
    glAttachShader(shader_program, frag_shader);

    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader_program, 512, NULL, infoLog);
        LOG_EF("Could not link shader: {}", infoLog);
    }

    mat->shader.render_data.shader_program = shader_program;
}

} // namespace blood