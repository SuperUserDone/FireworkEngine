#define GLAD_GL_IMPLEMENTATION

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include "core/logger.hpp"
#include "render/deletion_helpers.hpp"
#include "renderer.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"

namespace blood
{
renderer::renderer(const render_settings &p_render_settings) : m_settings(p_render_settings)
{
    create_window();
    create_gl_context();
    init_imgui();
    init_cameras();
}

void renderer::process_events()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
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

void renderer::clear_fb(framebuffer *fb)
{
    if (fb->fb_handle == -1)
        return;
    glBindFramebuffer(GL_FRAMEBUFFER, fb->fb_handle);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderer::render_imgui(void *state, std::function<bool()> callback)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(m_window);

    ImGui::NewFrame();

    if (callback())
        m_should_close = true;

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void renderer::render(double frametime,
                      const scene *scene,
                      const component_camera &cam,
                      const glm::mat4 &camera_transform,
                      framebuffer *fb,
                      glm::uvec2 size)
{

    // Camera stuff

    int x, y;
    if (size.x == 0 || size.y == 0)
    {
        SDL_GetWindowSize(m_window, &x, &y);
    }
    else
    {
        x = size.x;
        y = size.y;
    }

    glViewport(0, 0, x, y);
    setup_camera(cam, camera_transform, x, y);

    if (fb->fb_handle != 0)
    {
        if (fb->fb_handle == -1)
        {
            create_framebuffer(fb);
        }
        if (fb->size != glm::uvec2{x, y} || fb->texture_handle == -1)
        {
            fb->size = glm::uvec2{x, y};
            update_framebuffer_texture(fb);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fb->fb_handle);

    // Clear background
    glm::vec3 color = scene->m_back_color;
    glClearColor(color.r, color.g, color.b, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // Objects
    {
        // Fetch all meshes
        auto view = scene->m_entt.view<component_mesh, component_transform>();

        // Draw all meshes
        for (auto entity : view)
        {
            auto [mesh_comp, trans_comp] = view.get(entity);

            render_mesh(mesh_comp, trans_comp, cam);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderer::finish_render()
{
    // Swap buffers
    SDL_GL_SwapWindow(m_window);
}

renderer::~renderer()
{
    clean();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    // Destroy window and context
    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void renderer::init_cameras()
{
    // Generate buffer
    glGenBuffers(1, &m_camera_buffer);

    // Alocate buffer
    glBindBuffer(GL_UNIFORM_BUFFER, m_camera_buffer);
    glBufferData(GL_UNIFORM_BUFFER, 128, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void renderer::setup_camera(const component_camera &cam,
                            const glm::mat4 &camera_transform,
                            int x,
                            int y)
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_camera_buffer);

    // Set data
    glBufferSubData(
        GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(cam.get_projection(x, y)));
    glBufferSubData(GL_UNIFORM_BUFFER,
                    sizeof(glm::mat4),
                    sizeof(glm::mat4),
                    glm::value_ptr(glm::inverse((glm::mat4)camera_transform)));

    // Bind
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

    // Set attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // Create window
    m_window = SDL_CreateWindow(m_settings.name.c_str(),
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

    // Load data
    m_context = SDL_GL_CreateContext(m_window);
    if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "COULD NOT LOAD OPENGL", "ERROR", m_window);
        LOG_RUNTIME_ERROR("COULD NOT LOAD OPENGL");
    }

    if (!GLAD_GL_VERSION_4_2)
    {
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR, "COULD NOT LOAD OPENGL >= 4.2", "ERROR", m_window);
        LOG_RUNTIME_ERROR("COULD NOT LOAD OPENGL >= 4.2");
    }

    SDL_GL_MakeCurrent(m_window, m_context);
    SDL_GL_SetSwapInterval(m_settings.vsync);

    m_placeholder_mat = std::make_shared<blood::material>();

    m_placeholder_mat->shader.frag_source = R"(
#version 450 core
out vec4 FragColor;
in vec4 vertexColor;
void main() { FragColor = vertexColor; }
)";

    m_placeholder_mat->shader.vert_source = R"(
#version 450 core
layout(location = 0) in vec3 aPos;
layout(std140, binding = 0) uniform Matrices{
mat4 projection;
mat4 view;
};
layout(location = 0) uniform mat4 model;
out vec4 vertexColor;
void main() { gl_Position = projection * view * model * vec4(aPos, 1.0); vertexColor = vec4(0.5, 0.0, 0.0, 1.0); }
)";
}

void renderer::init_imgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    // io.ConfigViewportsNoAutoMerge = true;
    // io.ConfigViewportsNoTaskBarIcon = true;

    // Style
    ImGui::StyleColorsDark();

    io.Fonts->AddFontFromFileTTF("./font/NotoSans-Regular.ttf", 18.594061258312f);

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look
    // identical to regular ones.
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(m_window, m_context);
    ImGui_ImplOpenGL3_Init("#version 400");
}

void renderer::render_mesh(const component_mesh &mesh,
                           const component_transform &trans,
                           const component_camera &cam) const
{
    if (mesh.render_data.vao == -1)
        load_mesh(mesh, trans);

    if (mesh.changed)
        update_mesh(mesh, trans);

    if (!mesh.m_mat)
        mesh.m_mat = m_placeholder_mat;

    if (mesh.m_mat->shader.render_data.shader_program == -1)
        load_material(mesh.m_mat);

    // Use shader
    glUseProgram(mesh.m_mat->shader.render_data.shader_program);

    // Set transform
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr((glm::mat4)trans));

    // Set buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.render_data.index_buffer);
    glDrawElements(GL_TRIANGLES, mesh.indicies.size(), GL_UNSIGNED_INT, 0);
}

void renderer::load_mesh(const component_mesh &mesh, const component_transform &trans) const
{
    uint vao;
    uint vbo;
    uint ebo;

    // Create VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create VBO
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

    // VAO data info
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(vertex), (void *)offsetof(vertex, pos));
    glEnableVertexAttribArray(0);

    mesh.changed = false;
    mesh.render_data.index_buffer = ebo;
    mesh.render_data.vertex_buffer = vbo;
    mesh.render_data.vao = vao;
}

void renderer::update_mesh(const component_mesh &mesh, const component_transform &trans) const
{
    uint vao = mesh.render_data.vao;
    uint vbo = mesh.render_data.vertex_buffer;
    uint ebo = mesh.render_data.index_buffer;

    // Re-fill VAO
    glBindVertexArray(vao);

    // Re-fill VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER,
                 mesh.verticies.size() * sizeof(vertex),
                 mesh.verticies.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(uint32_t) * mesh.indicies.size(),
                 mesh.indicies.data(),
                 GL_STATIC_DRAW);

    // VAO data info
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

    glDeleteShader(frag_shader);
    glDeleteShader(vert_shader);

    mat->shader.render_data.shader_program = shader_program;
}

void renderer::create_framebuffer(framebuffer *fb) { glGenFramebuffers(1, (uint *)&fb->fb_handle); }

void renderer::update_framebuffer_texture(framebuffer *fb)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fb->fb_handle);

    if (fb->texture_handle == -1)
        glGenTextures(1, (uint *)&fb->texture_handle);

    glBindTexture(GL_TEXTURE_2D, fb->texture_handle);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, fb->size.x, fb->size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->texture_handle, 0);

    if (fb->renderbuffer_handle == -1)
        glGenRenderbuffers(1, (uint *)&fb->renderbuffer_handle);

    glBindRenderbuffer(GL_RENDERBUFFER, fb->renderbuffer_handle);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fb->size.x, fb->size.y);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb->renderbuffer_handle);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        LOG_F("Framebuffer is not complete!");

    glBindFramebuffer(GL_FRAMEBUFFER, fb->fb_handle);
}

void renderer::clean()
{
    while (!deletion_queue::get_queue().empty())
    {
        auto cmd = deletion_queue::get_queue().pop_queue();

        switch (cmd.type)
        {
        case deletion_command::GPU_BUFFER:
            glDeleteBuffers(1, &cmd.id);
            break;
        case deletion_command::GPU_VAO:
            glDeleteVertexArrays(1, &cmd.id);
            break;
        case deletion_command::GPU_SHADER_PROGRAM:
            glDeleteProgram(cmd.id);
            break;
        }
    }
}

} // namespace blood