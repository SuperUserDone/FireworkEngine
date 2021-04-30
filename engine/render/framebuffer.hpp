#pragma once

#include <stdint.h>

#include <glm/glm.hpp>

namespace blood
{

struct framebuffer
{
    int framebuffer = -1;
    int texture_handle = -1;
    int renderbuffer_handle = -1;

    glm::uvec2 size = {0, 0};
};

static inline framebuffer DEFAULT_FRAMEBUFFER = {0};

} // namespace blood
