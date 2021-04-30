#pragma once

#include <stdint.h>

#include <glm/glm.hpp>

namespace blood
{

struct fb_handle
{
    int fb_handle = -1;
    int texture_handle = -1;
    int renderbuffer_handle = -1;

    glm::uvec2 size = {0, 0};
};

static inline fb_handle DEFAULT_FRAMEBUFFER = fb_handle{0};

} // namespace blood
