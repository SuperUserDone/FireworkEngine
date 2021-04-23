#pragma once

#include <stddef.h>
#include <stdint.h>

namespace blood
{

struct render_settings
{
    /**
     * \brief The render api backend to use
     *
     */
    enum
    {
        RENDERER_OPENGL33
    } renderer_api = RENDERER_OPENGL33;

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
