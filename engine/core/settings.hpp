#pragma once

#include <stddef.h>
#include <stdint.h>

namespace blood
{

struct render_settings
{
    enum
    {
        RENDERER_OPENGL33
    } renderer_api = RENDERER_OPENGL33;

    uint32_t width = 800;
    uint32_t height = 600;

    bool vsync = true;
    int monitor = 0;
};

struct settings
{
    render_settings renderer;
};
} // namespace blood
