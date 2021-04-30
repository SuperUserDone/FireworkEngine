#pragma once

#include <stdint.h>

#include <glm/glm.hpp>

namespace blood
{

struct framebuffer
{
    int fb_handle = -1;
    int texture_handle = -1;
    int renderbuffer_handle = -1;

    glm::uvec2 size = {0, 0};
};

static inline framebuffer DEFAULT_FRAMEBUFFER = framebuffer{0};

} // namespace blood