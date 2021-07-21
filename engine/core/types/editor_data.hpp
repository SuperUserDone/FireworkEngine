#pragma once

#include "core/util.hpp"
#include "render/render_data/texture2d.hpp"

#include <glm/glm.hpp>

namespace fw {

struct editor_data
{
    float preview_size = 256.f;
    ref<texture2d> preview_texture = nullptr;
    bool dirty = true;
};

} // namespace fw