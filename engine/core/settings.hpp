#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string>

namespace blood {

enum renderer_impl
{
    RENDERER_NONE,
    RENDERER_OPENGL33
};

struct render_settings
{
    /**
     * \brief The render api backend to use
     *
     */
    renderer_impl renderer_api = RENDERER_OPENGL33;

    /**
     * \brief Screen size
     *
     */
    struct
    {
        uint32_t width = 800;
        uint32_t height = 600;
    };

    /**
     * \brief
     *
     */
    std::string name = "Blood";

    /**
     * \brief Enable vsync
     *
     */
    bool vsync = true;

    /**
     * \brief The monitor to apear on
     *
     */
    int monitor = 0;
};

struct settings
{
    render_settings renderer;
};
} // namespace blood
