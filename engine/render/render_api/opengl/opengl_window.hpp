#pragma once

#include "core/logger.hpp"
#include "core/settings.hpp"
#include "glad/gl.h"
#include "misc/freetype/imgui_freetype.h"
#include "render/fonts/JetBrains-Mono-Regular-Nerd.hpp"
#include "render/fonts/NotoSans-Regular.hpp"
#include "render/fonts/codepoint.hpp"

#include <SDL.h>
#include <Tracy.hpp>
#include <TracyOpenGL.hpp>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

namespace fw {
class opengl_window
{
public:
    opengl_window(const render_settings &settings) : m_settings(settings)
    {
        m_window = nullptr;

        FIREWORK_ASSERT(SDL_Init(SDL_INIT_VIDEO) == 0, "COULD NOT INIT SDL");

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

        // Create window
        m_window =
            SDL_CreateWindow(m_settings.name.c_str(),
                             SDL_WINDOWPOS_CENTERED_DISPLAY(m_settings.monitor),
                             SDL_WINDOWPOS_CENTERED_DISPLAY(m_settings.monitor),
                             m_settings.width,
                             m_settings.height,
                             SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);

        if (!m_window) {
            SDL_ShowSimpleMessageBox(
                SDL_MESSAGEBOX_ERROR, "COULD NOT INIT SDL", "ERROR CREATING WINDOW", m_window);
            LOG_RUNTIME_ERROR("COULD NOT INIT SDL");
        }

        LOG_I("Creating OpenGL context.");

        // Load data
        m_context = SDL_GL_CreateContext(m_window);
        if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress)) {
            SDL_ShowSimpleMessageBox(
                SDL_MESSAGEBOX_ERROR, "COULD NOT LOAD OPENGL", "ERROR", m_window);
            LOG_RUNTIME_ERROR("COULD NOT LOAD OPENGL");
        }

        if (!GLAD_GL_VERSION_4_5) {
            SDL_ShowSimpleMessageBox(
                SDL_MESSAGEBOX_ERROR, "COULD NOT LOAD OPENGL >= 4.5", "ERROR", m_window);
            LOG_RUNTIME_ERROR("COULD NOT LOAD OPENGL >= 4.5");
        }

        SDL_GL_MakeCurrent(m_window, m_context);
        SDL_GL_SetSwapInterval(m_settings.vsync);

        TracyGpuContext;

        init_imgui();
    }

    bool check_close() { return m_close; }

    void process_events()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            switch (event.type) {
            case SDL_QUIT:
                m_close = true;
                break;

            default:
                break;
            }
        }
    }

    void init_imgui()
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

        ImVector<ImWchar> ranges = get_ranges();
        ImVector<ImWchar> big_ranges = get_large_ranges();

        ImFontConfig config;
        config.MergeMode = true;

        io.Fonts->AddFontFromMemoryCompressedTTF(
            NotoSans_compressed_data, NotoSans_compressed_size, 18.f);

        io.Fonts->AddFontFromMemoryCompressedTTF(JetBrains_Mono_Regular_Nerd_compressed_data,
                                                 JetBrains_Mono_Regular_Nerd_compressed_size,
                                                 18.f,
                                                 &config,
                                                 ranges.Data);
        io.Fonts->AddFontFromMemoryCompressedTTF(JetBrains_Mono_Regular_Nerd_compressed_data,
                                                 JetBrains_Mono_Regular_Nerd_compressed_size,
                                                 128.f,
                                                 nullptr,
                                                 big_ranges.Data);
        io.Fonts->Build();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look
        // identical to regular ones.
        ImGuiStyle &style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForOpenGL(m_window, m_context);
        ImGui_ImplOpenGL3_Init("#version 450");
    }

    glm::uvec2 getsize()
    {
        int x, y;
        SDL_GetWindowSize(m_window, &x, &y);

        return glm::uvec2(x, y);
    }

    void render_imgui(std::function<bool()> callback) const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(m_window);

        ImGui::NewFrame();

        if (callback()) m_close = true;

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void swap_buffers()
    {
        SDL_GL_SwapWindow(m_window);
        TracyGpuCollect;
    }

    ~opengl_window()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        // Destroy window and context
        SDL_GL_DeleteContext(m_context);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }

private:
    SDL_Window *m_window;
    SDL_GLContext m_context;
    render_settings m_settings;
    mutable bool m_close = false;
};
} // namespace fw